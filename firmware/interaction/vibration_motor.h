#ifndef VIBRATION_MOTOR_H
#define VIBRATION_MOTOR_H

#include <cstdint>

class VibrationMotor {
public:
    VibrationMotor(uint8_t gate_pin = 19);

    void begin();
    void triggerAlertPulse(uint16_t pulse_ms = 300, uint8_t count = 3);
    void stop();
    bool isActive() const;

private:
    uint8_t gpio_pin;
    bool active;
};

#endif // VIBRATION_MOTOR_H
