# CareVoice Wearable --- Development Guidelines

## 1. General Rule

Build the smallest reliable implementation first.

Do not introduce unnecessary frameworks, services or dependencies.

## 2. Fall Detection

The fall detector is the highest-priority software component.

Do not implement:

-   velocity by naïvely integrating accelerometer data as the primary
    detector
-   a single-threshold-only fall detector
-   an untested ML model merely to make the project sound like AI

Prefer a multi-stage state machine using:

-   acceleration magnitude
-   impact characteristics
-   gyroscope/rotation
-   orientation change
-   post-event inactivity
-   temporal context

## 3. Accuracy Validation

Create a test dataset containing normal and fall-like activities.

At minimum test:

-   walking
-   sitting
-   standing
-   lying down
-   getting up
-   running
-   hand/arm movement
-   dropping the arm quickly
-   controlled/safe simulated falls

Report:

-   true positives
-   false positives
-   false negatives
-   true negatives
-   precision
-   recall
-   F1 score

Never hard-code a claimed accuracy percentage without test evidence.

## 4. Coding

-   Keep sensor drivers separate from business logic.
-   Keep fall detection as an isolated module.
-   Keep communication as an isolated module.
-   Keep hardware configuration centralized.
-   Use meaningful names.
-   Avoid duplicated logic.
-   Add logging for debugging.
-   Fail safely.

## 5. Configuration

Thresholds, Wi-Fi settings, API URL and device identity should be
configurable.

Do not hard-code secrets into Git.

## 6. Git

Use small commits.

Suggested commit style:

``` text
feat: add BMI270 sensor driver
feat: add fall detection state machine
feat: add SOS event
feat: add CareVoice API client
fix: handle Wi-Fi reconnect
test: add fall detection scenarios
```

## 7. Agent Behavior

Before changing existing CareVoice-Edge code:

1.  Inspect the existing implementation.
2.  Reuse existing models/services/components.
3.  Identify the smallest integration point.
4.  Make the change.
5.  Run tests.
6.  Verify the affected workflow.
7.  Explain changed files.

Do not rewrite working modules without a reason.
