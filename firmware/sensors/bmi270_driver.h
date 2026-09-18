#ifndef BMI270_DRIVER_H
#define BMI270_DRIVER_H

#include <cstdint>

struct AccelData {
    float x; // in g
    float y; // in g
    float z; // in g
};

struct GyroData {
    float x; // in deg/s
    float y; // in deg/s
    float z; // in deg/s
};

struct IMUData {
    AccelData accel;
    GyroData gyro;
    uint32_t timestamp_ms;
};

class BMI270Driver {
public:
    BMI270Driver(uint8_t i2c_addr = 0x68);
    
    bool begin(int sda_pin = 22, int scl_pin = 23);
    bool readSensorData(IMUData& out_data);
    float getAccelMagnitude(const AccelData& accel);
    
    void injectSimulatedData(const IMUData& sim_data);
    
private:
    uint8_t i2c_address;
    bool initialized;
    bool simulation_mode;
    IMUData current_sim_data;
};

#endif // BMI270_DRIVER_H
