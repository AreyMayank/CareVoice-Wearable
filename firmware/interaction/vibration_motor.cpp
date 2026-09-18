#include "vibration_motor.h"

VibrationMotor::VibrationMotor(uint8_t gate_pin)
    : gpio_pin(gate_pin), active(false) {}

void VibrationMotor::begin() {
    // GPIO output mode initialization placeholder
}

void VibrationMotor::triggerAlertPulse(uint16_t pulse_ms, uint8_t count) {
    (void)pulse_ms;
    (void)count;
    active = true;
    // Toggles MOSFET Gate pin high/low using PWM or digital write
}

void VibrationMotor::stop() {
    active = false;
    // Sets MOSFET Gate pin LOW
}

bool VibrationMotor::isActive() const {
    return active;
}
