# CareVoice Wearable --- End-to-End Workflow

## 1. Boot

``` text
Power ON
  |
Initialize ESP32-C6
  |
Initialize BMI270
  |
Initialize MAX30102
  |
Initialize button
  |
Initialize vibration
  |
Initialize audio
  |
Connect Wi-Fi
  |
Register/announce device
  |
Normal monitoring
```

## 2. Normal Monitoring

The wearable continuously monitors motion.

Heart-rate data is sampled according to the firmware configuration and
can be periodically transmitted.

Device state should include:

-   device_id
-   patient_id
-   battery
-   connectivity
-   heart rate when available
-   sensor status

## 3. Fall Detection

Use a state machine rather than a single threshold.

``` text
NORMAL
  |
  v
CANDIDATE
  |
  v
IMPACT CHECK
  |
  v
ORIENTATION CHECK
  |
  v
INACTIVITY CHECK
  |
  v
POSSIBLE_FALL
```

Thresholds must be treated as tunable parameters.

Do not claim a fall-detection accuracy percentage until it has been
measured using a defined test dataset.

## 4. Possible Fall

When a possible fall is detected:

1.  Generate `POSSIBLE_FALL`.
2.  Include fall confidence.
3.  Include available heart rate.
4.  Vibrate.
5.  Play voice prompt.
6.  Start 15-second timer.
7.  Inform CareVoice-Edge.

## 5. Patient Response

### Patient presses button

Generate:

`FALL_CANCELLED`

Reason:

`PATIENT_RESPONDED`

Stop escalation.

### Patient does not respond

Generate:

`EMERGENCY_FALL`

Reason:

`NO_RESPONSE`

CareVoice-Edge performs emergency escalation.

## 6. Manual SOS

Button press in normal state:

``` text
Button
  -> SOS
  -> immediate transmission
  -> CareVoice-Edge
  -> emergency workflow
```

No 15-second confirmation is required for manual SOS.

## 7. Dashboard Workflow

``` text
Wearable
   |
   v
FastAPI endpoint
   |
   +--> Validate payload
   |
   +--> Identify device/patient
   |
   +--> Store event
   |
   +--> Update live state
   |
   +--> Trigger alert engine when required
   |
   v
Caregiver dashboard
```

## 8. Offline Workflow

``` text
Wi-Fi lost
   |
Local monitoring continues
   |
Important event stored locally
   |
Retry connection
   |
Upload queued event
   |
Mark delivered
```

## 9. Development Workflow

``` text
Implement
  -> Unit test
  -> Hardware test
  -> Log sensor data
  -> Tune thresholds
  -> Controlled testing
  -> Integration test
  -> Dashboard verification
  -> Git commit
```
