#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <string>
#include <cstdint>

enum class EventType {
    STATUS_UPDATE,
    POSSIBLE_FALL,
    FALL_CANCELLED,
    EMERGENCY_FALL,
    SOS
};

struct EventPayload {
    std::string device_id;
    std::string patient_id;
    EventType event_type;
    float fall_confidence;
    uint8_t heart_rate;
    uint8_t battery_percent;
    std::string reason;
    std::string device_status;

    std::string toJsonString() const;
};

class EventManager {
public:
    EventManager(const std::string& dev_id = "wearable_001",
                 const std::string& pat_id = "patient_001",
                 uint32_t response_window_ms = 15000);

    EventPayload createStatusUpdate(uint8_t heart_rate, uint8_t battery, const std::string& status = "ONLINE");
    EventPayload createPossibleFall(float confidence, uint8_t heart_rate);
    EventPayload createFallCancelled(const std::string& reason = "PATIENT_RESPONDED");
    EventPayload createEmergencyFall(const std::string& reason = "NO_RESPONSE");
    EventPayload createSOS();

    void armConfirmationTimer(uint32_t current_time_ms);
    bool isTimerActive() const { return timer_active; }
    bool checkTimerExpired(uint32_t current_time_ms);
    void disarmTimer();

private:
    std::string device_id;
    std::string patient_id;
    uint32_t window_duration_ms;
    uint32_t timer_start_ms;
    bool timer_active;
};

#endif // EVENT_MANAGER_H
