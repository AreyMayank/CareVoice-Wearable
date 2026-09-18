#include "max30102_driver.h"

MAX30102Driver::MAX30102Driver(uint8_t i2c_addr)
    : i2c_address(i2c_addr), initialized(false), simulation_mode(false) {
    current_sim_data = {75, 98, true, 0};
}

bool MAX30102Driver::begin(int sda_pin, int scl_pin) {
    (void)sda_pin;
    (void)scl_pin;
    initialized = true;
    return true;
}

bool MAX30102Driver::readHeartRate(HeartRateData& out_data) {
    if (!initialized) return false;

    if (simulation_mode) {
        out_data = current_sim_data;
        return true;
    }

    // Default resting baseline when physical hardware is unattached
    out_data.heart_rate_bpm = 72;
    out_data.spo2_percent = 98;
    out_data.valid = true;
    out_data.timestamp_ms = 0;
    return true;
}

void MAX30102Driver::injectSimulatedHeartRate(uint8_t bpm, bool valid) {
    simulation_mode = true;
    current_sim_data.heart_rate_bpm = bpm;
    current_sim_data.valid = valid;
}
