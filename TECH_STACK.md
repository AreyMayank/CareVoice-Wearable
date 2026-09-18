# CareVoice Wearable --- Technology Stack

## 1. Wearable Hardware

  -----------------------------------------------------------------------
  Layer                               Technology
  ----------------------------------- -----------------------------------
  MCU                                 Seeed Studio XIAO ESP32-C6

  Fall sensor                         Bosch BMI270 6-axis IMU

  Heart-rate sensor                   MAX30102

  Audio amplifier                     MAX98357A I2S amplifier

  Speaker                             8Ω mini speaker

  Haptic                              3V coin vibration motor

  Motor driver                        AO3400A N-MOSFET

  Motor protection                    1N5819 Schottky diode

  Input                               Tactile SOS/OK button

  Power                               3.7V Li-Po battery

  Indicator                           Status LED

  Charging                            Use the selected XIAO board's
                                      supported battery/charging
                                      arrangement
  -----------------------------------------------------------------------

## 2. Firmware

Primary language:

-   C/C++ with the ESP32 development framework selected for the project

Firmware responsibilities:

-   GPIO
-   I2C
-   I2S
-   Wi-Fi
-   Sensor acquisition
-   Fall-detection state machine
-   Timers
-   Event serialization
-   HTTP/HTTPS communication
-   Local buffering

## 3. Existing CareVoice-Edge

### Backend

-   Python
-   FastAPI
-   SQLAlchemy
-   SQLite/PostgreSQL support as already implemented in the project
-   JWT/authentication where applicable

### Frontend

-   React
-   TypeScript
-   Vite

### Integrations already present in the project

-   Emergency alert engine
-   Twilio
-   Telegram
-   ntfy push
-   SMTP/email

Use the existing project integrations rather than creating duplicate
notification systems.

## 4. Development Tools

-   Antigravity IDE
-   VS Code-compatible workflow where required
-   Git
-   GitHub
-   Python virtual environment for backend work

## 5. Communication

Prototype communication:

``` text
ESP32-C6 -> Wi-Fi -> CareVoice-Edge FastAPI
```

Use structured JSON event payloads.

## 6. Important Hardware Rule

Never assume a breakout-board pinout from its product name alone.

Before final soldering, verify:

-   Exact ESP32-C6 board
-   Exact BMI270 breakout
-   Exact MAX30102 breakout
-   Exact MAX98357A breakout
-   Voltage requirements
-   Pin labels
-   Onboard regulators/level shifting
-   Battery/charging arrangement

The software agent must not invent a pin mapping.
