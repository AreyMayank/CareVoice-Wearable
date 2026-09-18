#include "wifi_client.h"

WiFiClientManager::WiFiClientManager(const std::string& ssid,
                                     const std::string& password,
                                     const std::string& server_url)
    : wifi_ssid(ssid), wifi_pass(password), target_url(server_url),
      connected_state(false), simulation_mode(false) {}

bool WiFiClientManager::connect() {
    if (simulation_mode) {
        return connected_state;
    }
    // Baseline network setup
    connected_state = true;
    return true;
}

bool WiFiClientManager::isConnected() const {
    return connected_state;
}

bool WiFiClientManager::sendEventPayload(const EventPayload& payload) {
    if (!connected_state) return false;

    // HTTP POST payload transmission stub (uses ESP32 HTTPClient in firmware build)
    std::string json_data = payload.toJsonString();
    (void)json_data;
    return true;
}

void WiFiClientManager::simulateConnectionStatus(bool connected) {
    simulation_mode = true;
    connected_state = connected;
}
