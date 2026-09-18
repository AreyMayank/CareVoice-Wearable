from fastapi.testclient import TestClient
from backend.mock_server import app

client = TestClient(app)

def test_status_update_event():
    payload = {
        "device_id": "wearable_001",
        "patient_id": "patient_001",
        "event": "STATUS_UPDATE",
        "heart_rate": 76,
        "battery": 82,
        "device_status": "ONLINE"
    }
    response = client.post("/api/v1/wearable/events", json=payload)
    assert response.status_code == 201
    data = response.json()
    assert data["status"] == "success"
    assert "server_timestamp" in data

def test_possible_fall_event():
    payload = {
        "device_id": "wearable_001",
        "patient_id": "patient_001",
        "event": "POSSIBLE_FALL",
        "fall_confidence": 0.87,
        "heart_rate": 82
    }
    response = client.post("/api/v1/wearable/events", json=payload)
    assert response.status_code == 201

def test_fall_cancelled_event():
    payload = {
        "device_id": "wearable_001",
        "patient_id": "patient_001",
        "event": "FALL_CANCELLED",
        "reason": "PATIENT_RESPONDED"
    }
    response = client.post("/api/v1/wearable/events", json=payload)
    assert response.status_code == 201

def test_emergency_fall_event():
    payload = {
        "device_id": "wearable_001",
        "patient_id": "patient_001",
        "event": "EMERGENCY_FALL",
        "reason": "NO_RESPONSE"
    }
    response = client.post("/api/v1/wearable/events", json=payload)
    assert response.status_code == 201
    
    # Verify live device alert state updated
    status_resp = client.get("/api/v1/wearable/status/wearable_001")
    assert status_resp.status_code == 200
    assert status_resp.json()["active_alert"] == "EMERGENCY_FALL"

def test_sos_event():
    payload = {
        "device_id": "wearable_001",
        "patient_id": "patient_001",
        "event": "SOS"
    }
    response = client.post("/api/v1/wearable/events", json=payload)
    assert response.status_code == 201

def test_invalid_event_type():
    payload = {
        "device_id": "wearable_001",
        "patient_id": "patient_001",
        "event": "INVALID_EVENT"
    }
    response = client.post("/api/v1/wearable/events", json=payload)
    assert response.status_code == 400

def test_list_events_and_telemetry():
    response = client.get("/api/v1/wearable/events")
    assert response.status_code == 200
    events = response.json()["events"]
    assert len(events) >= 5
