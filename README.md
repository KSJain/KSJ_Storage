# KSJ_Storage

Reusable persistence for Arduino-compatible embedded systems.

> **Storage is an observer.**

Its responsibility is to preserve engineering observations.

It never controls the application.

It never becomes required for the application to remain safe.

If storage fails, the system continues operating.

---

## Philosophy

> Observation before optimization.

> Every observation should be useful more than once.

KSJ_Storage exists to turn temporary device observations into durable engineering evidence.

Applications should think in terms of persistence and records—not filesystem-driver details.

---

## Current Release

**v0.1.0**

Development toward **v0.2.0** is in progress.

---

## Existing Features

- SD-card mounting
- Card detection
- Card-type reporting
- Capacity and usage information
- Text-file writing
- Text-file appending
- Text-file reading
- Structured failure reporting
- Hardware-independent text-storage interface

The existing `v0.1.0` API remains supported.

---

## Session Logging

The upcoming `v0.2.0` architecture introduces append-only session logs.

Each boot or experiment can create an independent session containing:

- Human-readable session identity
- Persistent boot count
- Firmware version
- Board identity
- Zero-based uptime
- Monotonic record sequence
- Telemetry records
- Event records
- Session lifecycle records

Example JSON Lines:

```json
{"session":"PB1-23","seq":0,"uptime_ms":0,"type":"session_start","data":{"boot_count":23,"firmware":"0.7.0","board":"Prototype Box v1"}}
{"session":"PB1-23","seq":1,"uptime_ms":5000,"type":"telemetry","data":{"ldr_raw":2610,"stable":true}}
{"session":"PB1-23","seq":2,"uptime_ms":7800,"type":"event","event":"LIGHT_CHANGED","data":{"from":"BRIGHT","to":"DIM"}}

## Development

See [`docs/DEVELOPING_KSJ_LIBRARIES.md`](docs/DEVELOPING_KSJ_LIBRARIES.md) for the local-example and released-library workflow.