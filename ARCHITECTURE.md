# CareVoice Wearable --- System Architecture

## 1. High-Level Architecture

``` text
                PATIENT WRIST
                     |
        +------------+------------+
        |                         |
     BMI270                    MAX30102
    6-axis IMU               Heart Rate
        |                         |
        +------------+------------+
                     |
                ESP32-C6
              Local Controller
        +------------+------------+
        |      |       |          |
     Button  Motor   Audio      Wi-Fi
        |      |       |          |
        |   MOSFET  MAX98357A     |
        |      |       |          |
        |  Vibration Speaker      |
        |                         |
        +------------+------------+
                     |
                  Wi-Fi
                     |
                     v
             CareVoice-Edge
                     |
          +----------+----------+
          |          |          |
       FastAPI    Database   Dashboard
          |
      Alert Engine
          |
   Calls / Telegram /
   Push / Email
```

## 2. Responsibility Split

### Wearable firmware

Responsible for:

-   Reading BMI270
-   Reading MAX30102
-   Local fall-detection logic
-   Fall confidence calculation
-   Post-fall inactivity
-   15-second timer
-   Button handling
-   Vibration control
-   Voice prompts
-   Wi-Fi connection
-   Event creation
-   Retry/buffering when connectivity is unavailable

### CareVoice-Edge backend

Responsible for:

-   Device registration
-   Patient-device association
-   Receiving wearable events
-   Persisting events
-   Updating dashboard state
-   Alert escalation
-   Caregiver notifications
-   Event history

### Frontend

Responsible for:

-   Live device state
-   Heart rate
-   Battery
-   Fall events
-   SOS events
-   Event history
-   Caregiver-facing status and alerts

## 3. Data Flow

``` text
Sensor
  -> ESP32-C6
  -> Local processing
  -> Event generated
  -> Wi-Fi
  -> FastAPI
  -> Database
  -> Dashboard
  -> Alert engine when required
```

## 4. Design Rule

Do not continuously stream raw IMU data to the server by default.

The wearable should process motion locally and send useful
summaries/events. Raw sensor capture can be enabled as a controlled
debugging/data-collection mode.

## 5. Failure Handling

### Wi-Fi unavailable

-   Continue local monitoring.
-   Do not stop fall detection.
-   Buffer important events locally.
-   Retry transmission when connectivity returns.

### Server unavailable

-   Continue local monitoring.
-   Keep the patient interaction workflow functional.
-   Retry event delivery.

### Sensor unavailable

-   Mark the relevant sensor/device state as degraded.
-   Do not silently report fabricated sensor values.

## 6. Security

-   Never hard-code production secrets.
-   Keep Wi-Fi credentials outside source control.
-   Use environment/configuration variables.
-   Authenticate device-to-server communication.
-   Validate all incoming hardware payloads.
-   Reject malformed or unknown device events.
