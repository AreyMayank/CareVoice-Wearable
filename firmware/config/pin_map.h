#ifndef PIN_MAP_H
#define PIN_MAP_H

/**
 * CareVoice Wearable - XIAO ESP32-C6 Pin Assignment Mapping
 * Note: Check physical breakout labels before powering hardware.
 */

// I2C Shared Bus Pins (BMI270 IMU + MAX30102 Heart Rate)
#ifndef PIN_I2C_SDA
#define PIN_I2C_SDA 22
#endif

#ifndef PIN_I2C_SCL
#define PIN_I2C_SCL 23
#endif

// I2C Sensor Addresses
#define BMI270_I2C_ADDR   0x68 // Primary I2C address for BMI270 (0x69 if SDO pulled high)
#define MAX30102_I2C_ADDR 0x57 // Standard I2C address for MAX30102

// I2S Audio Bus Pins (MAX98357A Digital Audio Amplifier)
#define PIN_I2S_BCLK 16 // Bit Clock
#define PIN_I2S_LRCK 17 // Left/Right Word Select Clock
#define PIN_I2S_DOUT 18 // Data Out to Amplifier

// Vibration Motor Driver Pin (AO3400A N-Channel MOSFET Gate)
#define PIN_VIBRATION_GATE 19

// Tactile Push Button Pin (SOS / Cancel OK action)
#define PIN_TACTILE_BUTTON 20

// Status Indicator LED Pin
#define PIN_STATUS_LED 15

#endif // PIN_MAP_H
