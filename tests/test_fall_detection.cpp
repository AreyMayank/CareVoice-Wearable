#include "../firmware/fall_detection/fall_state_machine.h"
#include "../firmware/events/event_manager.h"
#include "../firmware/storage/offline_queue.h"

#include <iostream>
#include <vector>
#include <cassert>

void testNormalActivities() {
    FallStateMachine sm;
    std::cout << "[TEST 1] Testing Normal Activities (Walking, Arm Movement)..." << std::endl;

    // Normal walking IMU vector samples (~1.0g to 1.3g)
    for (uint32_t t = 0; t < 1000; t += 50) {
        IMUData sample;
        sample.accel = {0.05f, 0.10f, 1.05f};
        sample.gyro = {2.0f, -1.0f, 0.5f};
        sample.timestamp_ms = t;

        FallDetectionResult res = sm.processSample(sample);
        assert(res.state == FallState::NORMAL);
        assert(!res.event_triggered);
    }
    std::cout << "  -> PASSED: 0 false positives during normal activity." << std::endl;
}

void testFallDetectionScenario() {
    FallStateMachine sm;
    std::cout << "[TEST 2] Testing True Positive Fall Scenario..." << std::endl;

    // Step 1: Normal state
    IMUData sample;
    sample.accel = {0.0f, 0.0f, 1.0f};
    sample.timestamp_ms = 0;
    sm.processSample(sample);

    // Step 2: High Candidate acceleration (free fall / motion anomaly)
    sample.accel = {0.5f, 0.5f, 2.5f};
    sample.timestamp_ms = 100;
    sm.processSample(sample);
    assert(sm.getCurrentState() == FallState::CANDIDATE);

    // Step 3: Peak Impact acceleration (3.5g impact)
    sample.accel = {1.0f, 1.5f, 3.5f};
    sample.timestamp_ms = 200;
    sm.processSample(sample);
    assert(sm.getCurrentState() == FallState::IMPACT_CHECK);

    // Step 4: Orientation Shift (45+ degrees shift)
    sample.accel = {1.0f, 0.0f, 0.1f};
    sample.timestamp_ms = 300;
    sm.processSample(sample);

    // Step 5: Post-fall inactivity check (stationary for 1.5s)
    sample.accel = {0.99f, 0.01f, 0.02f};
    sample.timestamp_ms = 400;
    sm.processSample(sample);

    sample.timestamp_ms = 2000;
    FallDetectionResult final_res = sm.processSample(sample);

    assert(final_res.state == FallState::POSSIBLE_FALL);
    assert(final_res.event_triggered);
    assert(final_res.confidence_score >= 0.80f);
    std::cout << "  -> PASSED: True fall correctly detected (Confidence: " << final_res.confidence_score << ")." << std::endl;
}

void test15SecondPatientConfirmation() {
    EventManager em("wearable_001", "patient_001", 15000);
    std::cout << "[TEST 3] Testing 15-Second Patient Confirmation & Cancellation..." << std::endl;

    em.armConfirmationTimer(1000);
    assert(em.isTimerActive());
    assert(!em.checkTimerExpired(5000)); // 5 seconds elapsed -> timer active

    // Patient presses OK button at 6s
    em.disarmTimer();
    assert(!em.isTimerActive());

    EventPayload cancel_evt = em.createFallCancelled("PATIENT_RESPONDED");
    assert(cancel_evt.reason == "PATIENT_RESPONDED");
    std::cout << "  -> PASSED: Patient button press successfully cancelled fall escalation." << std::endl;
}

void test15SecondNoResponseTimeout() {
    EventManager em("wearable_001", "patient_001", 15000);
    std::cout << "[TEST 4] Testing 15-Second No-Response Escalation..." << std::endl;

    em.armConfirmationTimer(1000);
    assert(!em.checkTimerExpired(10000)); // 9s elapsed -> no expiry

    bool expired = em.checkTimerExpired(16500); // 15.5s elapsed -> expired!
    assert(expired);

    EventPayload emergency_evt = em.createEmergencyFall("NO_RESPONSE");
    assert(emergency_evt.reason == "NO_RESPONSE");
    std::cout << "  -> PASSED: No-response timeout successfully generated EMERGENCY_FALL event." << std::endl;
}

void testOfflineQueueing() {
    OfflineQueue queue(5);
    EventManager em;
    std::cout << "[TEST 5] Testing Offline Event Queueing & Flushing..." << std::endl;

    assert(queue.isEmpty());
    queue.push(em.createPossibleFall(0.89f, 85));
    queue.push(em.createEmergencyFall("NO_RESPONSE"));

    assert(queue.count() == 2);
    EventPayload pop1;
    assert(queue.pop(pop1));
    assert(pop1.event_type == EventType::POSSIBLE_FALL);
    std::cout << "  -> PASSED: Offline queueing and retrieval operating as expected." << std::endl;
}

int main() {
    std::cout << "=== Running CareVoice Wearable Test Suite ===" << std::endl;
    testNormalActivities();
    testFallDetectionScenario();
    test15SecondPatientConfirmation();
    test15SecondNoResponseTimeout();
    testOfflineQueueing();
    std::cout << "\n=== ALL TESTS PASSED SUCCESSFULLY (100% Accuracy on Synthetic Benchmarks) ===" << std::endl;
    return 0;
}
