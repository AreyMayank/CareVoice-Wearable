#ifndef FALL_STATE_MACHINE_H
#define FALL_STATE_MACHINE_H

#include "../sensors/bmi270_driver.h"
#include <cstdint>

enum class FallState {
    NORMAL,
    CANDIDATE,
    IMPACT_CHECK,
    ORIENTATION_CHECK,
    INACTIVITY_CHECK,
    POSSIBLE_FALL,
    FALL_CANCELLED
};

struct FallDetectionResult {
    FallState state;
    bool event_triggered;
    float confidence_score;
};

class FallStateMachine {
public:
    FallStateMachine(float accel_candidate_thresh = 2.2f,
                      float accel_impact_thresh = 3.0f,
                      float orientation_change_deg = 45.0f,
                      float inactivity_tolerance = 0.25f);

    void reset();
    FallDetectionResult processSample(const IMUData& sample);

    FallState getCurrentState() const { return current_state; }
    float getFallConfidence() const { return confidence; }

private:
    float candidate_thresh;
    float impact_thresh;
    float orientation_thresh;
    float inactivity_tol;

    FallState current_state;
    float confidence;

    uint32_t candidate_time_ms;
    uint32_t impact_time_ms;
    uint32_t inactivity_start_ms;

    AccelData baseline_accel;
    AccelData impact_accel;
    
    float calculateAngleDeltaDeg(const AccelData& v1, const AccelData& v2);
};

#endif // FALL_STATE_MACHINE_H
