# CareVoice Wearable --- Patient Safety & Fall Detection System

<p align="center">
  <b>CareVoice Wearable</b> is a wrist-worn patient-safety device built on the <b>Seeed Studio XIAO ESP32-C6</b> MCU. It detects falls locally with high precision, prompts the patient for confirmation via audio and haptic feedback, and communicates real-time emergency events to the <b>CareVoice-Edge</b> backend.
</p>

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [System Architecture](#system-architecture)
- [Hardware Components & BOM](#hardware-components--bom)
- [Tech Stack](#tech-stack)
- [Fall Detection & 15-Second Response State Machine](#fall-detection--15-second-response-state-machine)
- [API Contract & Event Schema](#api-contract--event-schema)
- [Repository Structure](#repository-structure)
- [Installation & Running Guide](#installation--running-guide)
  - [Prerequisites](#prerequisites)
  - [Backend Setup](#1-backend-setup)
  - [Firmware Build & Flashing](#2-firmware-build--flashing)
  - [Running Automated Tests](#3-running-automated-tests)
- [License](#license)

---

## Overview

CareVoice Wearable operates as an edge sensor node for the existing **CareVoice-Edge** ecosystem. To optimize network bandwidth and ensure instantaneous reaction times, fall detection algorithms, post-fall inactivity verification, patient response windows, and audio/haptic alert triggers are executed locally on the microcontroller.

- **Primary Goal**: High-accuracy fall detection with minimal false alarms.
- **Secondary Goal**: Heart-rate monitoring, local audio/haptic guidance, and offline event buffering.

---

## Key Features

- **Multi-Stage Local Fall Detection**: Evaluates 6-axis IMU acceleration magnitude, peak impact, rotation/orientation delta, and post-fall inactivity.
- **15-Second Patient Confirmation Window**: Triggers a local voice prompt ("*Fall detected. Press button if you are okay*") and vibration pulse, giving the patient 15 seconds to cancel false alarms locally.
- **Automatic Emergency Escalation**: If the patient fails to respond within 15 seconds, an `EMERGENCY_FALL` alert is automatically dispatched to CareVoice-Edge for caregiver intervention.
- **Immediate Manual SOS**: Physical push button press during normal operation instantly dispatches an un-delayed `SOS` emergency alert.
- **Heart-Rate Monitoring**: Periodic sampling and telemetry reporting of patient pulse via MAX30102 PPG sensor.
- **Offline Event Buffering**: Automatically queues alerts in non-volatile storage when Wi-Fi is lost and flushes the queue upon network recovery.

---

## System Architecture

```text
               +---------------------------------------------------+
               |                   PATIENT WRIST                   |
               +-------------------------+-------------------------+
                                         |
                       +-----------------+-----------------+
                       |                                   |
                  Bosch BMI270                        MAX30102
                6-axis IMU (I2C)                  Heart Rate (I2C)
                       |                                   |
                       +-----------------+-----------------+
                                         |
                                  Seeed ESP32-C6
                             (Local Edge Processing)
                       +-----------------+-----------------+
                       |         |       |                 |
                    Tactile   Vibration I2S Audio        Wi-Fi
                    Button    Motor (MOSFET) (MAX98357A)   |
                       |         |       |                 |
                       +---------+-------+                 |
                                                           v
                                                     Wi-Fi / HTTP
                                                           |
                                                           v
                                                    CareVoice-Edge
                                                    FastAPI Backend
                                                           |
                                                    +------+------+
                                                    |             |
                                                Database      Caregiver
                                                (SQLite/PG)   Dashboard
```

---

## Hardware Components & BOM

| Component | Part / Module | Function / Protocol |
| :--- | :--- | :--- |
| **Microcontroller (MCU)** | Seeed Studio XIAO ESP32-C6 | Core MCU managing I2C, I2S, GPIO interrupts, Wi-Fi, & State Machine |
| **6-Axis IMU** | Bosch BMI270 | Accelerometer + Gyroscope for motion & fall detection (I2C `0x68`) |
| **Heart-Rate Sensor** | MAX30102 | Optical Heart Rate & $\text{SpO}_2$ pulse oximeter (I2C `0x57`) |
| **Audio Amplifier** | MAX98357A | I2S Digital Class-D Audio Amplifier |
| **Speaker** | 8Ω 1W Mini Speaker | Outputs local audio guidance and voice prompts |
| **Haptic Motor Driver** | AO3400A N-FET + 1N5819 | Drives 3V coin vibration motor with flyback protection |
| **Tactile Push Button** | Tactile Switch | OK/Cancel confirmation during 15s window; Immediate SOS during normal state |
| **Power System** | 3.7V Li-Po Battery (500-1000mAh) | Managed via native XIAO ESP32-C6 charging controller |

---

## Tech Stack

- **Firmware**: C / C++ (ESP32 / PlatformIO / Arduino Framework)
- **Sensors**: I2C bus driver modules for BMI270 & MAX30102
- **Audio/Haptics**: I2S PCM audio output, GPIO N-FET PWM/Digital driver
- **Backend API**: Python 3.10+, FastAPI, Pydantic v2, Uvicorn
- **Testing**: `pytest`, `TestClient`, C++ benchmark test suite

---

## Fall Detection & 15-Second Response State Machine

```text
                  [ Normal Monitoring ]
                            |
                     Motion Anomaly
                            |
                            v
                   [ Candidate Check ]  (|a| > 2.2g)
                            |
                      Impact Check      (|a| > 3.0g)
                            |
                     Orientation Delta  (> 45 deg)
                            |
                   Inactivity Check     (1.5s stillness)
                            |
                            v
                 [ POSSIBLE_FALL Event ]
                 - Send low-priority notification
                 - Trigger Vibration + Voice Prompt
                 - Start 15-Second Timer
                            |
                +-----------+-----------+
                |                       |
          Button Pressed          Timer Expired
                |                       |
                v                       v
       [ FALL_CANCELLED ]      [ EMERGENCY_FALL ]
      (Reason: RESPONDED)     (Reason: NO_RESPONSE)
```

---

## API Contract & Event Schema

Events dispatched from the wearable to CareVoice-Edge (`POST /api/v1/wearable/events`):

### 1. `POSSIBLE_FALL`
```json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "POSSIBLE_FALL",
  "fall_confidence": 0.87,
  "heart_rate": 82
}
```

### 2. `FALL_CANCELLED`
```json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "FALL_CANCELLED",
  "reason": "PATIENT_RESPONDED"
}
```

### 3. `EMERGENCY_FALL`
```json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "EMERGENCY_FALL",
  "reason": "NO_RESPONSE"
}
```

### 4. `SOS` (Immediate Manual Emergency)
```json
{
  "device_id": "wearable_001",
  "patient_id": "patient_001",
  "event": "SOS"
}
```

---

## Repository Structure

```text
CareVoice-Wearable/
├── firmware/
│   ├── config/
│   │   ├── config.h            # Operational parameters & thresholds
│   │   └── pin_map.h           # ESP32-C6 GPIO pin assignments
│   ├── sensors/
│   │   ├── bmi270_driver.h/cpp # 6-axis IMU driver
│   │   └── max30102_driver.h/cpp # Heart Rate driver
│   ├── interaction/
│   │   ├── button_handler.h/cpp # Debounced button controller
│   │   ├── vibration_motor.h/cpp # MOSFET haptic driver
│   │   └── audio_prompt.h/cpp   # I2S audio driver
│   ├── fall_detection/
│   │   └── fall_state_machine.h/cpp # Multi-stage fall detection
│   ├── events/
│   │   └── event_manager.h/cpp # JSON payload & 15s timer manager
│   ├── connectivity/
│   │   └── wifi_client.h/cpp   # Wi-Fi HTTP client
│   ├── storage/
│   │   └── offline_queue.h/cpp # Offline event persistence buffer
│   └── main.cpp                # Application entry point
├── backend/
│   └── mock_server.py          # FastAPI event ingestion server
├── tests/
│   ├── test_fall_detection.cpp # C++ state machine benchmarks
│   └── test_mock_server.py     # Pytest backend validation suite
├── ABOUT.md                    # Detailed architectural analysis
├── API_CONTRACT.md             # Canonical API contract specification
├── HARDWARE.md                 # Hardware assembly & schematic documentation
└── README.md                   # Main project documentation
```

---

## Installation & Running Guide

### Prerequisites

- **Python 3.10+** installed
- **Git** installed
- (Optional for Hardware) **PlatformIO / VS Code** for flashing the ESP32-C6 microcontroller

---

### 1. Backend Setup

Clone the repository and install backend dependencies:

```bash
git clone https://github.com/AreyMayank/CareVoice-Wearable.git
cd CareVoice-Wearable

# Install required Python packages
pip install fastapi uvicorn pydantic pytest httpx
```

Run the backend event ingestion server locally:

```bash
python -m uvicorn backend.mock_server:app --reload --host 0.0.0.0 --port 8000
```

The API docs will be available at `http://localhost:8000/docs`.

---

### 2. Firmware Build & Flashing

1. Open the repository root in **PlatformIO** (or your ESP32 build environment).
2. Configure Wi-Fi credentials and server endpoint in [`firmware/config/config.h`](file:///d:/Antigravity_files/fall_wrist/firmware/config/config.h).
3. Connect your **Seeed Studio XIAO ESP32-C6** via USB-C.
4. Verify GPIO pin assignments in [`firmware/config/pin_map.h`](file:///d:/Antigravity_files/fall_wrist/firmware/config/pin_map.h).
5. Build and upload:
   ```bash
   pio run --target upload
   ```

---

### 3. Running Automated Tests

Run the Pytest suite for backend schema validation and event handling:

```bash
python -m pytest tests/test_mock_server.py
```

Expected output:
```text
======================== 7 passed in 0.66s =========================
```

---

## License

This project is licensed under the MIT License.
