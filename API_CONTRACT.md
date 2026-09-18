# CareVoice Wearable --- API/Event Contract

## 1. Purpose

The wearable communicates with CareVoice-Edge using structured events.

The backend should validate every payload.

## 2. Device Status

``` json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "STATUS_UPDATE",
  "heart_rate": 76,
  "battery": 82,
  "device_status": "ONLINE"
}
```

## 3. Possible Fall

``` json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "POSSIBLE_FALL",
  "fall_confidence": 0.87,
  "heart_rate": 82
}
```

## 4. Fall Cancelled

``` json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "FALL_CANCELLED",
  "reason": "PATIENT_RESPONDED"
}
```

## 5. Emergency Fall

``` json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "EMERGENCY_FALL",
  "reason": "NO_RESPONSE"
}
```

## 6. SOS

``` json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "SOS"
}
```

## 7. Backend Rules

-   Validate device identity.
-   Validate patient association.
-   Validate event type.
-   Validate numeric ranges.
-   Add server-side timestamp.
-   Store event history.
-   Avoid duplicate processing when the same event is retried.
-   Return a clear success/failure response.
