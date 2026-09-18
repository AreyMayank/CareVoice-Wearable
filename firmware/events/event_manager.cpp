#include "event_manager.h"
#include <sstream>
#include <iomanip>

std::string EventPayload::toJsonString() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"device_id\": \"" << device_id << "\",\n";
    ss << "  \"patient_id\": \"" << patient_id << "\",\n";

    switch (event_type) {
        case EventType::STATUS_UPDATE:
            ss << "  \"event\": \"STATUS_UPDATE\",\n";
            ss << "  \"heart_rate\": " << (int)heart_rate << ",\n";
            ss << "  \"battery\": " << (int)battery_percent << ",\n";
            ss << "  \"device_status\": \"" << device_status << "\"\n";
            break;

        case EventType::POSSIBLE_FALL:
            ss << "  \"event\": \"POSSIBLE_FALL\",\n";
            ss << "  \"fall_confidence\": " << std::fixed << std::setprecision(2) << fall_confidence << ",\n";
            ss << "  \"heart_rate\": " << (int)heart_rate << "\n";
            break;

        case EventType::FALL_CANCELLED:
            ss << "  \"event\": \"FALL_CANCELLED\",\n";
            ss << "  \"reason\": \"" << reason << "\"\n";
            break;

        case EventType::EMERGENCY_FALL:
            ss << "  \"event\": \"EMERGENCY_FALL\",\n";
            ss << "  \"reason\": \"" << reason << "\"\n";
            break;

        case EventType::SOS:
            ss << "  \"event\": \"SOS\"\n";
            break;
    }

    ss << "}";
    return ss.str();
}

EventManager::EventManager(const std::string& dev_id, const std::string& pat_id, uint32_t response_window_ms)
    : device_id(dev_id), patient_id(pat_id), window_duration_ms(response_window_ms),
      timer_start_ms(0), timer_active(false) {}

EventPayload EventManager::createStatusUpdate(uint8_t heart_rate, uint8_t battery, const std::string& status) {
    EventPayload payload;
    payload.device_id = device_id;
    payload.patient_id = patient_id;
    payload.event_type = EventType::STATUS_UPDATE;
    payload.heart_rate = heart_rate;
    payload.battery_percent = battery;
    payload.device_status = status;
    return payload;
}

EventPayload EventManager::createPossibleFall(float confidence, uint8_t heart_rate) {
    EventPayload payload;
    payload.device_id = device_id;
    payload.patient_id = patient_id;
    payload.event_type = EventType::POSSIBLE_FALL;
    payload.fall_confidence = confidence;
    payload.heart_rate = heart_rate;
    return payload;
}

EventPayload EventManager::createFallCancelled(const std::string& reason) {
    EventPayload payload;
    payload.device_id = device_id;
    payload.patient_id = patient_id;
    payload.event_type = EventType::FALL_CANCELLED;
    payload.reason = reason;
    return payload;
}

EventPayload EventManager::createEmergencyFall(const std::string& reason) {
    EventPayload payload;
    payload.device_id = device_id;
    payload.patient_id = patient_id;
    payload.event_type = EventType::EMERGENCY_FALL;
    payload.reason = reason;
    return payload;
}

EventPayload EventManager::createSOS() {
    EventPayload payload;
    payload.device_id = device_id;
    payload.patient_id = patient_id;
    payload.event_type = EventType::SOS;
    return payload;
}

void EventManager::armConfirmationTimer(uint32_t current_time_ms) {
    timer_start_ms = current_time_ms;
    timer_active = true;
}

bool EventManager::checkTimerExpired(uint32_t current_time_ms) {
    if (!timer_active) return false;
    if (current_time_ms - timer_start_ms >= window_duration_ms) {
        timer_active = false;
        return true;
    }
    return false;
}

void EventManager::disarmTimer() {
    timer_active = false;
}
