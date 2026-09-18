# About the CareVoice Wearable Project

## 1. Executive Summary & Project Goal

**CareVoice Wearable** is a wrist-worn patient-safety hardware device designed to complement the existing **CareVoice-Edge** platform. Its primary objective is to accurately detect patient falls locally, prompt the user for confirmation, and transmit real-time alert events to the CareVoice-Edge backend via Wi-Fi.

Key design principles of the project:
* **Input Device Model**: The wearable serves as an intelligent hardware sensor node for CareVoice-Edge, **not** a standalone platform. It reuses existing CareVoice-Edge backend services, alert engines, database schemas, and frontend dashboards.
* **Fall Detection First**: Fall detection accuracy and reliability are the top priorities.
* **Edge Processing**: Motion processing, candidate fall evaluations, post-fall inactivity checks, audio/haptic prompts, and the 15-second response timer are handled locally on the ESP32-C6 microcontroller to minimize network bandwidth and prevent dependency on persistent server connectivity.
* **Patient Autonomy & False Alarm Reduction**: A 15-second response window allows patients to cancel false alarms locally before emergency escalation occurs.

---

## 2. Core Features

1. **Multi-Stage Local Fall Detection**: Uses a 6-axis IMU (Bosch BMI270) analyzing acceleration magnitude, impact, orientation changes, and post-fall inactivity.
2. **Patient Confirmation Window**: 15-second window triggered upon detecting a candidate fall, using local vibration (motor) and voice prompts (speaker) to ask for confirmation.
3. **Cancellation / OK Action**: Patients can press the manual physical button during the confirmation window to report safety (`FALL_CANCELLED`).
4. **No-Response Escalation**: If the patient fails to respond within 15 seconds, the wearable escalates the event to `EMERGENCY_FALL`, triggering the CareVoice-Edge alert engine.
5. **Immediate SOS**: Pressing the physical button during normal monitoring immediately sends an un-delayed `SOS` emergency alert to CareVoice-Edge.
6. **Heart-Rate Monitoring**: Periodic sampling and reporting of heart rate via MAX30102 sensor.
7. **Local Audio & Haptic Feedback**: I2S audio amplifier (MAX98357A) driving an 8Ω speaker for voice alerts, alongside an N-FET driven 3V coin vibration motor.
8. **Offline Event Buffering**: Local event queue and retry mechanism on the wearable ensuring critical alerts are safely stored and transmitted once Wi-Fi re-connects.
9. **Device & Battery Telemetry**: Regular `STATUS_UPDATE` reporting heart rate, battery percentage (3.7V Li-Po), and online state.

---

## 3. Hardware Architecture & BOM

| Component Category | Part / Module | Description / Protocol |
| :--- | :--- | :--- |
| **Microcontroller (MCU)** | Seeed Studio XIAO ESP32-C6 | Core MCU managing I2C, I2S, GPIO, Wi-Fi, timers, & local state machine |
| **Motion / Fall Sensor** | Bosch BMI270 | 6-axis IMU (Accelerometer + Gyroscope) connected via I2C |
| **Heart Rate Sensor** | MAX30102 | Optical Heart-Rate sensor sharing the I2C bus with BMI270 |
| **Audio Amplifier** | MAX98357A | I2S Digital Class-D Audio Amplifier |
| **Audio Output** | 8Ω 1W Mini Speaker | Outputs local voice guidance and alerts |
| **Haptic Driver** | AO3400A N-Channel MOSFET + 1N5819 Diode | Drives 3V coin vibration motor with flyback protection |
| **User Input** | Tactile Push Button | Serves as OK/Cancel during 15s confirmation & SOS trigger during normal state |
| **Power System** | 3.7V Li-Po Battery (500-1000mAh) | Managed by native XIAO ESP32-C6 power & charging circuitry |

---

## 4. Software & Firmware Architecture

### Firmware Structure (`firmware/`)
* `main`: Initialization sequence (sensors, audio, button, Wi-Fi) and main loop execution.
* `sensors/`: Isolated drivers for `bmi270` (motion/fall vectors) and `max30102` (heart rate).
* `fall_detection/`: Multi-stage state machine handling candidate falls, impact checks, orientation changes, and post-fall inactivity.
* `interaction/`: GPIO controllers for tactile button, N-FET vibration motor, and I2S audio playback.
* `connectivity/`: Wi-Fi management, HTTP/HTTPS client, and reconnection logic.
* `events/`: Event creation, serialization, and JSON payload formatting.
* `storage/`: Local flash/NVS event queue for offline retries.
* `config/`: Centralized hardware pin definitions, thresholds, Wi-Fi credentials, and server URLs.

### CareVoice-Edge Integration
* **Backend**: Python / FastAPI app with SQLAlchemy database models (SQLite/PostgreSQL), auth modules, and multi-channel alert dispatchers (Twilio, Telegram, ntfy push, SMTP).
* **Frontend**: React + TypeScript + Vite dashboard rendering real-time device telemetry, patient status, and active emergency fall/SOS notifications.

---

## 5. Event API & Communication Contract

Communication from the ESP32-C6 to CareVoice-Edge FastAPI backend occurs via JSON HTTP/HTTPS POST payloads:

1. **`STATUS_UPDATE`**: Sent periodically to report heart rate, battery, and online status.
2. **`POSSIBLE_FALL`**: Sent immediately upon local candidate fall detection (low-priority alert; initiates local 15-second patient window).
3. **`FALL_CANCELLED`**: Sent when patient presses the OK button during the 15-second window (`reason: PATIENT_RESPONDED`).
4. **`EMERGENCY_FALL`**: Sent when the 15-second window expires without patient response (`reason: NO_RESPONSE`), triggering urgent caregiver escalation.
5. **`SOS`**: Sent immediately when the button is pressed during normal operation (bypasses confirmation window).

---

## 6. Operational Workflows & Fall State Machine

```
                        [ Boot & Initialization ]
                                    |
                                    v
                          [ Normal Monitoring ]
                                 /     \
                     Motion Anomaly   Manual Button Press
                               /         \
                              v           v
                    [ Candidates Check ]   [ Immediate SOS Event ]
                    - Acceleration Vector
                    - Impact Peak
                    - Rotation & Orientation
                    - Post-Fall Inactivity
                              |
                              v
                    [ POSSIBLE_FALL Event ]
                    - Notify CareVoice-Edge (Low Priority)
                    - Trigger Local Vibration & Voice Alert
                    - Start 15-Second Timer
                              |
                     +--------+--------+
                     |                 |
             Patient Button      15s Timeout
                     |                 |
                     v                 v
            [ FALL_CANCELLED ]  [ EMERGENCY_FALL ]
            (Cancel Escalation) (Caregiver Emergency Alert)
```

---

## 7. Key Constraints & Development Rules

1. **No Duplicate Platforms**: Do not build a standalone server, alternative backend, or separate web dashboard.
2. **No Hardware Pin Assumptions**: Physical pin mappings must be verified against actual breakout board schematics before finalizing code.
3. **Measured Accuracy**: Fall detection accuracy must be empirically validated using precision, recall, and F1-score against recorded test datasets (walking, sitting, lying down, controlled falls, arm drops) rather than hard-coded claims.
4. **No Raw Stream Overload**: Motion is processed on-device; raw IMU streams are only transmitted in dedicated debugging/data collection modes.
5. **Security & Configuration**: Environment variables/config files must be used for Wi-Fi credentials and API keys. Production secrets must never be committed into Git.
