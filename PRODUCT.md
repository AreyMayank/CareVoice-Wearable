# CareVoice Wearable --- Product Specification

## 1. Product Overview

CareVoice Wearable is a compact wrist-worn patient-safety device that
detects possible falls locally, asks the patient for confirmation, and
sends the resulting event to the existing CareVoice-Edge platform.

The wearable is a hardware input device for CareVoice-Edge. It is **not
a separate backend or dashboard**.

### Primary objective

Detect falls with high practical accuracy and inform CareVoice-Edge.

### Secondary objective

Provide heart-rate monitoring and local patient feedback.

## 2. Target Users

-   Elderly users
-   Home-care patients
-   Hospital/care-facility patients
-   Users requiring fall monitoring

## 3. Main Environment

-   Indoor use
-   Home
-   Hospital
-   Care facilities

## 4. Core Features

1.  Automatic fall detection
2.  Multi-signal fall decision using accelerometer + gyroscope
3.  Post-fall inactivity check
4.  15-second patient confirmation window
5.  Patient OK/cancel button
6.  No-response emergency escalation
7.  Immediate manual SOS
8.  Voice alerts
9.  Vibration feedback
10. Heart-rate monitoring
11. Wi-Fi connectivity
12. CareVoice-Edge event transmission
13. Device online/offline status
14. Battery status
15. Event history
16. Compact rechargeable wearable

## 5. Fall Response Logic

``` text
Normal Monitoring
      |
      v
Motion anomaly detected
      |
      v
Impact + orientation + inactivity checks
      |
      v
Possible Fall
      |
      +--> Notify CareVoice-Edge as low-priority event
      |
      +--> Vibrate + voice prompt
      |
      v
15-second response window
      |
   +--+--+
   |     |
  OK    No response
   |     |
   v     v
Cancel  Emergency Fall
          |
          v
   CareVoice-Edge escalation
```

## 6. SOS Logic

The manual SOS button is an immediate emergency action.

``` text
SOS button pressed
      |
      v
ESP32-C6
      |
      v
SOS event
      |
      v
CareVoice-Edge
      |
      v
Emergency alert workflow
```

The SOS action does not use the 15-second fall confirmation window.

## 7. Product Principles

-   Accuracy first for fall detection
-   Local/edge processing where practical
-   Minimal unnecessary data transmission
-   Simple patient interaction
-   Reliable event delivery
-   Clear separation between wearable firmware and CareVoice-Edge
    backend
-   Prototype first; optimize hardware size after validation

## 8. V1 Scope

V1 should focus on:

-   Fall detection
-   15-second confirmation
-   SOS
-   Vibration
-   Voice prompt
-   Heart rate
-   Wi-Fi
-   CareVoice-Edge integration
-   Battery/device status

GPS, GSM, smartphone app, advanced cloud AI, waterproofing and custom
PCB are future scope unless explicitly added.

## 9. Non-Goals

Do not add features merely because they are technically possible.

Do not introduce GPS/GSM/cloud services into V1 without a product
requirement.

Do not replace the existing CareVoice-Edge dashboard/backend with a
second platform.
