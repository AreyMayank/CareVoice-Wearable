# CareVoice Wearable --- Hardware Specification

## 1. Prototype BOM

### Main electronics

-   1 × Seeed Studio XIAO ESP32-C6
-   1 × BMI270 6-axis IMU breakout
-   1 × MAX30102 breakout
-   1 × MAX98357A I2S amplifier module
-   1 × 8Ω 1W mini speaker
-   1 × 3V coin vibration motor
-   1 × AO3400A N-channel MOSFET
-   1 × 1N5819 Schottky diode
-   1 × tactile push button
-   1 × 3.7V Li-Po battery, approximately 500--1000mAh
-   1 × status LED
-   1 × power switch

### Passive/support components

-   1 × 220Ω resistor
-   1 × 100Ω resistor
-   1 × 10kΩ resistor
-   2 × 4.7kΩ resistors for I2C pull-ups if not already present on the
    modules
-   1 × 100µF electrolytic capacitor
-   1--3 × 0.1µF ceramic capacitors
-   Hook-up wires
-   Perfboard/prototype PCB
-   USB-C cable
-   Wrist strap/enclosure

## 2. Sensor Bus

BMI270 and MAX30102 can share the I2C bus when their addresses and
electrical configuration permit it.

``` text
ESP32 I2C SDA ----+---- BMI270 SDA
                  |
                  +---- MAX30102 SDA

ESP32 I2C SCL ----+---- BMI270 SCL
                  |
                  +---- MAX30102 SCL
```

## 3. Motor Driver

``` text
Battery/approved motor supply
        |
      Motor
        |
        +---- AO3400A Drain
              Source ---- GND

ESP32 GPIO -- 100Ω -- Gate
Gate -- 10kΩ -- GND

1N5819 is placed across the motor with correct polarity.
```

## 4. Audio

``` text
ESP32-C6
  |
  | I2S
  v
MAX98357A
  |
  v
8Ω Speaker
```

## 5. Power

Use the exact power/charging arrangement supported by the selected XIAO
ESP32-C6 board.

Do not add a TP4056 or separate 3.3V regulator automatically.

Do not connect a Li-Po directly to a generic 3.3V-only input.

## 6. Soldering Rule

Prototype on breadboard/perfboard first.

Before applying battery power:

-   Check polarity
-   Check continuity
-   Check for shorts between power and GND
-   Verify every module's voltage rating
-   Verify module pin labels
-   Power modules one section at a time
