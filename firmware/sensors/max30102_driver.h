#ifndef MAX30102_DRIVER_H
#define MAX30102_DRIVER_H

#include <cstdint>

struct HeartRateData {
    uint8_t heart_rate_bpm;
    uint8_t spo2_percent;
    bool valid;
    uint32_t timestamp_ms;
};

class MAX30102Driver {
public:
    MAX30102Driver(uint8_t i2c_addr = 0x57);

    bool begin(int sda_pin = 22, int scl_pin = 23);
    bool readHeartRate(HeartRateData& out_data);
    void injectSimulatedHeartRate(uint8_t bpm, bool valid = true);

private:
    uint8_t i2c_address;
    bool initialized;
    bool simulation_mode;
    HeartRateData current_sim_data;
};

#endif // MAX30102_DRIVER_H
