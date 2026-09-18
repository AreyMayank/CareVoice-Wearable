#include "config/config.h"
#include "sensors/bmi270_driver.h"
#include "sensors/max30102_driver.h"
#include "interaction/button_handler.h"
#include "interaction/vibration_motor.h"
#include "interaction/audio_prompt.h"
#include "fall_detection/fall_state_machine.h"
#include "events/event_manager.h"
#include "connectivity/wifi_client.h"
#include "storage/offline_queue.h"

#include <iostream>
#include <chrono>
#include <thread>

class CareVoiceWearableApp {
public:
    CareVoiceWearableApp()
        : bmi270(BMI270_I2C_ADDR),
          max30102(MAX30102_I2C_ADDR),
          button(PIN_TACTILE_BUTTON, DEBOUNCE_DELAY_MS),
          vibration(PIN_VIBRATION_GATE),
          audio(PIN_I2S_BCLK, PIN_I2S_LRCK, PIN_I2S_DOUT),
          fall_sm(ACCEL_CANDIDATE_THRESHOLD_G, ACCEL_IMPACT_THRESHOLD_G, ORIENTATION_CHANGE_DEG, INACTIVITY_ACCEL_TOLERANCE),
          event_mgr(DEFAULT_DEVICE_ID, DEFAULT_PATIENT_ID, FALL_CONFIRMATION_WINDOW_MS),
          wifi(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD, DEFAULT_SERVER_URL),
          offline_queue(OFFLINE_EVENT_QUEUE_SIZE),
          current_time_ms(0),
          last_telemetry_ms(0) {}

    void setup() {
        std::cout << "[CareVoice Wearable] Booting firmware on Seeed Studio XIAO ESP32-C6..." << std::endl;
        bmi270.begin(PIN_I2C_SDA, PIN_I2C_SCL);
        max30102.begin(PIN_I2C_SDA, PIN_I2C_SCL);
        button.begin();
        vibration.begin();
        audio.begin();
        wifi.connect();
        std::cout << "[CareVoice Wearable] Initialization complete. Entering normal monitoring state." << std::endl;
    }

    void step(uint32_t simulated_ms_delta = 100) {
        current_time_ms += simulated_ms_delta;

        // 1. Read Motion & Heart Rate Sensors
        IMUData imu_sample;
        bmi270.readSensorData(imu_sample);
        imu_sample.timestamp_ms = current_time_ms;

        HeartRateData hr_sample;
        max30102.readHeartRate(hr_sample);

        // 2. Process Fall Detection State Machine
        FallDetectionResult fall_res = fall_sm.processSample(imu_sample);

        if (fall_res.event_triggered && fall_res.state == FallState::POSSIBLE_FALL) {
            std::cout << "[ALERT] Candidate fall confirmed locally (Confidence: " 
                      << fall_res.confidence_score << "). Triggering patient confirmation." << std::endl;
            
            EventPayload payload = event_mgr.createPossibleFall(fall_res.confidence_score, hr_sample.heart_rate_bpm);
            dispatchOrQueueEvent(payload);

            vibration.triggerAlertPulse(300, 3);
            audio.playPrompt(AudioPromptType::FALL_CONFIRMATION_ALERT);
            event_mgr.armConfirmationTimer(current_time_ms);
        }

        // 3. Check Patient Response Window Expiration
        if (event_mgr.isTimerActive() && event_mgr.checkTimerExpired(current_time_ms)) {
            std::cout << "[ALERT] 15-second window expired without patient response! Escalating to EMERGENCY_FALL." << std::endl;
            EventPayload payload = event_mgr.createEmergencyFall("NO_RESPONSE");
            dispatchOrQueueEvent(payload);
            vibration.stop();
            audio.stop();
        }

        // 4. Handle Tactile Push Button Events
        ButtonEvent b_event = button.update(current_time_ms);
        if (b_event == ButtonEvent::SHORT_PRESS) {
            if (event_mgr.isTimerActive()) {
                std::cout << "[INFO] Patient pressed OK button during response window. Cancelling fall escalation." << std::endl;
                event_mgr.disarmTimer();
                fall_sm.reset();
                vibration.stop();
                audio.playPrompt(AudioPromptType::CANCEL_ACKNOWLEDGED);

                EventPayload payload = event_mgr.createFallCancelled("PATIENT_RESPONDED");
                dispatchOrQueueEvent(payload);
            } else {
                std::cout << "[EMERGENCY] Manual SOS button pressed during normal monitoring!" << std::endl;
                audio.playPrompt(AudioPromptType::SOS_ACTIVATED);
                EventPayload payload = event_mgr.createSOS();
                dispatchOrQueueEvent(payload);
            }
        }

        // 5. Periodic Telemetry Status Broadcast
        if (current_time_ms - last_telemetry_ms >= TELEMETRY_INTERVAL_MS) {
            last_telemetry_ms = current_time_ms;
            EventPayload payload = event_mgr.createStatusUpdate(hr_sample.heart_rate_bpm, 85, "ONLINE");
            dispatchOrQueueEvent(payload);
        }

        // 6. Drain Offline Queue if Wi-Fi Connected
        drainOfflineQueue();
    }

    void simulateButtonPress(ButtonEvent event) {
        button.simulatePress(event);
    }

    void injectSimulatedMotion(const IMUData& imu_data) {
        bmi270.injectSimulatedData(imu_data);
    }

private:
    void dispatchOrQueueEvent(const EventPayload& payload) {
        if (wifi.isConnected()) {
            std::cout << "[NET] Sending JSON payload -> Server: " << payload.toJsonString() << std::endl;
            wifi.sendEventPayload(payload);
        } else {
            std::cout << "[NET] Wi-Fi offline. Queuing payload in flash buffer." << std::endl;
            offline_queue.push(payload);
        }
    }

    void drainOfflineQueue() {
        if (wifi.isConnected() && !offline_queue.isEmpty()) {
            EventPayload payload;
            while (offline_queue.pop(payload)) {
                std::cout << "[NET-RETRY] Flushing offline event to server..." << std::endl;
                wifi.sendEventPayload(payload);
            }
        }
    }

    BMI270Driver bmi270;
    MAX30102Driver max30102;
    ButtonHandler button;
    VibrationMotor vibration;
    AudioPrompt audio;
    FallStateMachine fall_sm;
    EventManager event_mgr;
    WiFiClientManager wifi;
    OfflineQueue offline_queue;

    uint32_t current_time_ms;
    uint32_t last_telemetry_ms;
};

int main() {
    CareVoiceWearableApp app;
    app.setup();
    // Example tick
    app.step(100);
    return 0;
}
