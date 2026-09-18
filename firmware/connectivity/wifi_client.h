#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include "../events/event_manager.h"
#include <string>

class WiFiClientManager {
public:
    WiFiClientManager(const std::string& ssid = "CareVoice_WiFi",
                      const std::string& password = "pass",
                      const std::string& server_url = "http://127.0.0.1:8000/api/v1/wearable/events");

    bool connect();
    bool isConnected() const;
    bool sendEventPayload(const EventPayload& payload);

    void simulateConnectionStatus(bool connected);

private:
    std::string wifi_ssid;
    std::string wifi_pass;
    std::string target_url;
    bool connected_state;
    bool simulation_mode;
};

#endif // WIFI_CLIENT_H
