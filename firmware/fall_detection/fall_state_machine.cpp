#include "fall_state_machine.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

FallStateMachine::FallStateMachine(float accel_candidate_thresh,
                                     float accel_impact_thresh,
                                     float orientation_change_deg,
                                     float inactivity_tolerance)
    : candidate_thresh(accel_candidate_thresh),
      impact_thresh(accel_impact_thresh),
      orientation_thresh(orientation_change_deg),
      inactivity_tol(inactivity_tolerance),
      current_state(FallState::NORMAL),
      confidence(0.0f),
      candidate_time_ms(0),
      impact_time_ms(0),
      inactivity_start_ms(0) {
    baseline_accel = {0.0f, 0.0f, 1.0f};
    impact_accel = {0.0f, 0.0f, 1.0f};
}

void FallStateMachine::reset() {
    current_state = FallState::NORMAL;
    confidence = 0.0f;
    candidate_time_ms = 0;
    impact_time_ms = 0;
    inactivity_start_ms = 0;
}

float FallStateMachine::calculateAngleDeltaDeg(const AccelData& v1, const AccelData& v2) {
    float mag1 = std::sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
    float mag2 = std::sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);
    if (mag1 < 0.001f || mag2 < 0.001f) return 0.0f;

    float dot = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z) / (mag1 * mag2);
    dot = std::max(-1.0f, std::min(1.0f, dot));
    float angle_rad = std::acos(dot);
    return angle_rad * (180.0f / M_PI);
}

FallDetectionResult FallStateMachine::processSample(const IMUData& sample) {
    FallDetectionResult result;
    result.state = current_state;
    result.event_triggered = false;
    result.confidence_score = confidence;

    float mag = std::sqrt(sample.accel.x * sample.accel.x +
                          sample.accel.y * sample.accel.y +
                          sample.accel.z * sample.accel.z);

    switch (current_state) {
        case FallState::NORMAL:
            // Update continuous baseline
            baseline_accel = sample.accel;
            if (mag >= candidate_thresh) {
                current_state = FallState::CANDIDATE;
                candidate_time_ms = sample.timestamp_ms;
            }
            break;

        case FallState::CANDIDATE:
            if (mag >= impact_thresh) {
                current_state = FallState::IMPACT_CHECK;
                impact_accel = sample.accel;
                impact_time_ms = sample.timestamp_ms;
            } else if (sample.timestamp_ms - candidate_time_ms > 500) {
                // Timeout candidate check if no impact follows
                reset();
            }
            break;

        case FallState::IMPACT_CHECK: {
            float angle_change = calculateAngleDeltaDeg(baseline_accel, impact_accel);
            if (angle_change >= orientation_thresh) {
                current_state = FallState::ORIENTATION_CHECK;
                inactivity_start_ms = sample.timestamp_ms;
            } else if (sample.timestamp_ms - impact_time_ms > 1000) {
                reset();
            }
            break;
        }

        case FallState::ORIENTATION_CHECK:
            current_state = FallState::INACTIVITY_CHECK;
            break;

        case FallState::INACTIVITY_CHECK: {
            float diff = std::abs(mag - 1.0f);
            if (diff <= inactivity_tol) {
                if (sample.timestamp_ms - inactivity_start_ms >= 1500) {
                    current_state = FallState::POSSIBLE_FALL;
                    confidence = 0.88f;
                    result.event_triggered = true;
                }
            } else if (sample.timestamp_ms - inactivity_start_ms > 3000) {
                // High activity detected after impact -> reset
                reset();
            }
            break;
        }

        case FallState::POSSIBLE_FALL:
            // Remains in POSSIBLE_FALL until reset or confirmation workflow resolves it
            break;

        case FallState::FALL_CANCELLED:
            break;
    }

    result.state = current_state;
    result.confidence_score = confidence;
    return result;
}
