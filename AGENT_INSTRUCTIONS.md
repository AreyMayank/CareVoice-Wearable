# CareVoice --- Antigravity IDE Agent Instructions

## Role

You are the engineering agent for the CareVoice Wearable +
CareVoice-Edge project.

Your job is to implement reliable, maintainable software and hardware
integration.

## Project Context

CareVoice consists of:

1.  A wrist-worn ESP32-C6 wearable.
2.  The existing CareVoice-Edge backend.
3.  The existing CareVoice-Edge frontend/dashboard.

The wearable is a data/event source for CareVoice-Edge.

## Highest Priority

**Fall-detection reliability is the highest-priority requirement.**

The product must not be treated as a demo-only sensor project.

## Before Coding

Always inspect the repository.

Identify:

-   backend entry points
-   existing database models
-   existing alert engine
-   existing emergency event types
-   existing patient/caretaker models
-   existing API routing
-   existing frontend dashboard components
-   existing tests

Reuse existing architecture wherever possible.

## Do Not

-   Create a second backend.
-   Create a second dashboard.
-   Duplicate the alert engine.
-   Add unnecessary cloud infrastructure.
-   Add GPS/GSM without explicit approval.
-   Replace existing working functionality unnecessarily.
-   Invent hardware pin mappings.
-   Claim fall-detection accuracy without measured testing.
-   Store secrets in source code.
-   Delete existing features without approval.

## Hardware Assumptions

Current intended hardware:

-   XIAO ESP32-C6
-   BMI270
-   MAX30102
-   MAX98357A
-   8Ω speaker
-   3V vibration motor
-   AO3400A MOSFET
-   1N5819 diode
-   SOS/OK button
-   3.7V Li-Po

Exact breakout-board pinouts must be verified before firmware pin
definitions are finalized.

## Software Architecture

Keep modules separated:

``` text
firmware/
├── main
├── sensors/
│   ├── bmi270
│   └── max30102
├── fall_detection/
├── interaction/
│   ├── button
│   ├── vibration
│   └── audio
├── connectivity/
├── events/
├── storage/
└── config/
```

The exact directory structure may be adapted to the existing repository.

## Fall State Machine

Use:

``` text
NORMAL
  -> CANDIDATE
  -> IMPACT_CHECK
  -> ORIENTATION_CHECK
  -> INACTIVITY_CHECK
  -> POSSIBLE_FALL
  -> WAIT_FOR_RESPONSE
  -> FALL_CANCELLED

or

WAIT_FOR_RESPONSE
  -> EMERGENCY_FALL
```

Manual SOS is independent:

``` text
BUTTON
  -> SOS
```

## Event Names

Use these canonical event names unless the existing backend already
defines an equivalent convention:

-   `STATUS_UPDATE`
-   `POSSIBLE_FALL`
-   `FALL_CANCELLED`
-   `EMERGENCY_FALL`
-   `SOS`

## 15-Second Rule

A possible fall produces a low-priority event.

The patient gets approximately 15 seconds to respond.

If the patient presses the OK/SOS button in the fall-response state:

`FALL_CANCELLED`

If there is no response:

`EMERGENCY_FALL`

Manual SOS is immediate.

## Implementation Discipline

For every meaningful change:

1.  Inspect.
2.  Plan.
3.  Implement.
4.  Test.
5.  Review.
6.  Report.

When a hardware-dependent feature cannot be physically tested, clearly
label it as hardware-unverified.

## Documentation

Update relevant Markdown documentation when architecture, API, hardware
or workflow changes.

Keep documentation factual and synchronized with the actual
implementation.

## Definition of Done

A feature is not complete merely because the code compiles.

It should have:

-   implementation
-   error handling
-   relevant tests
-   logging where useful
-   documentation
-   integration verification where possible
