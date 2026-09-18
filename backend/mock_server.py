import time
from typing import Optional, List, Dict, Any
from fastapi import FastAPI, HTTPException, Body, status
from pydantic import BaseModel, Field

app = FastAPI(
    title="CareVoice-Edge Wearable API Mock Server",
    description="Backend endpoint service for CareVoice Wearable event ingestion and validation",
    version="1.0.0"
)

# In-memory storage for events and live device status
event_store: List[Dict[str, Any]] = []
device_state: Dict[str, Dict[str, Any]] = {}

class WearableEventSchema(BaseModel):
    device_id: str = Field(..., json_schema_extra={"example": "wearable_001"})
    patient_id: str = Field(..., json_schema_extra={"example": "patient_001"})
    event: str = Field(..., json_schema_extra={"example": "POSSIBLE_FALL"})
    fall_confidence: Optional[float] = Field(None, ge=0.0, le=1.0)
    heart_rate: Optional[int] = Field(None, ge=30, le=240)
    battery: Optional[int] = Field(None, ge=0, le=100)
    device_status: Optional[str] = Field(None, json_schema_extra={"example": "ONLINE"})
    reason: Optional[str] = Field(None, json_schema_extra={"example": "PATIENT_RESPONDED"})

@app.post("/api/v1/wearable/events", status_code=status.HTTP_201_CREATED)
def receive_wearable_event(payload: WearableEventSchema = Body(...)):
    """
    Ingests, validates, and processes incoming events from the CareVoice Wearable.
    Canonical events: STATUS_UPDATE, POSSIBLE_FALL, FALL_CANCELLED, EMERGENCY_FALL, SOS
    """
    canonical_events = {"STATUS_UPDATE", "POSSIBLE_FALL", "FALL_CANCELLED", "EMERGENCY_FALL", "SOS"}
    if payload.event not in canonical_events:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Invalid event type '{payload.event}'. Allowed: {canonical_events}"
        )

    # Server timestamp assignment
    timestamp = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    record = payload.model_dump(exclude_none=True)
    record["received_at"] = timestamp

    # Deduplication / State Store update
    event_store.append(record)
    
    device_id = payload.device_id
    if device_id not in device_state:
        device_state[device_id] = {
            "device_id": device_id,
            "patient_id": payload.patient_id,
            "last_seen": timestamp,
            "status": "ONLINE"
        }
    
    device_state[device_id]["last_seen"] = timestamp
    if payload.heart_rate is not None:
        device_state[device_id]["heart_rate"] = payload.heart_rate
    if payload.battery is not None:
        device_state[device_id]["battery"] = payload.battery
    if payload.event in {"EMERGENCY_FALL", "SOS"}:
        device_state[device_id]["active_alert"] = payload.event

    return {
        "status": "success",
        "message": f"Event '{payload.event}' recorded for patient '{payload.patient_id}'",
        "server_timestamp": timestamp
    }

@app.get("/api/v1/wearable/events")
def list_events(limit: int = 50):
    return {"events": event_store[-limit:]}

@app.get("/api/v1/wearable/status/{device_id}")
def get_device_status(device_id: str):
    if device_id not in device_state:
        raise HTTPException(status_code=404, detail="Device not found")
    return device_state[device_id]

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
