#include "bmi270_driver.h"
#include <cmath>

BMI270Driver::BMI270Driver(uint8_t i2c_addr)
    : i2c_address(i2c_addr), initialized(false), simulation_mode(false) {
    current_sim_data = {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, 0};
}

bool BMI270Driver::begin(int sda_pin, int scl_pin) {
    (void)sda_pin;
    (void)scl_pin;
    // Hardware initialization placeholder (I2C register configuration for BMI270)
    initialized = true;
    return true;
}

bool BMI270Driver::readSensorData(IMUData& out_data) {
    if (!initialized) return false;

    if (simulation_mode) {
        out_data = current_sim_data;
        return true;
    }

    // Baseline static data placeholder when physical I2C hardware is unattached
    out_data.accel.x = 0.02f;
    out_data.accel.y = -0.01f;
    out_data.accel.z = 0.98f;
    out_data.gyro.x = 0.1f;
    out_data.gyro.y = 0.0f;
    out_data.gyro.z = -0.1f;
    out_data.timestamp_ms = 0; // Filled by caller or sensor clock
    return true;
}

float BMI270Driver::getAccelMagnitude(const AccelData& accel) {
    return std::sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
}

void BMI270Driver::injectSimulatedData(const IMUData& sim_data) {
    simulation_mode = true;
    current_sim_data = sim_data;
}
