#ifndef CONFIG_H
#define CONFIG_H

#include "pin_map.h"

/**
 * CareVoice Wearable Configuration Parameters
 */

// Device Identifiers
#define DEFAULT_DEVICE_ID  "wearable_001"
#define DEFAULT_PATIENT_ID "patient_001"

// Network & API Endpoints
#define DEFAULT_WIFI_SSID     "CareVoice_Home_WiFi"
#define DEFAULT_WIFI_PASSWORD "SecurePass123"
#define DEFAULT_SERVER_URL    "http://192.168.1.100:8000/api/v1/wearable/events"

// Fall Detection Algorithm Thresholds (Tunable parameters)
#define ACCEL_CANDIDATE_THRESHOLD_G 2.2f  // Acceleration magnitude threshold to trigger candidate state
#define ACCEL_IMPACT_THRESHOLD_G    3.0f  // Severe impact magnitude threshold
#define ORIENTATION_CHANGE_DEG     45.0f  // Orientation angular shift threshold after impact
#define INACTIVITY_ACCEL_TOLERANCE  0.25f // Max acceleration deviation considered stationary/inactive
#define INACTIVITY_WINDOW_MS       2000   // Duration of post-fall inactivity check

// Response Window & Timers
#define FALL_CONFIRMATION_WINDOW_MS 15000 // 15-second patient OK response window
#define DEBOUNCE_DELAY_MS           50    // Button debounce threshold
#define HEART_RATE_SAMPLE_INTERVAL_MS 5000 // HR sampling interval
#define TELEMETRY_INTERVAL_MS       30000 // Status telemetry broadcast interval

// Offline Event Storage Queue Size
#define OFFLINE_EVENT_QUEUE_SIZE 20

#endif // CONFIG_H
