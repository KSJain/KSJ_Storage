KSJ_Storage v0.2.0

Append-only session memory for embedded observations

KSJ_Storage v0.2.0 expands the original SD text-storage brick into a session-oriented observation system for Arduino-compatible ESP32 devices.

Storage is an observer.

It preserves engineering observations without controlling the application or becoming required for the application to remain safe.

Highlights

One append-only JSON object per line.

A new, independently named session file for each boot.

Session numbering inferred from files already present on the card.

Sequence numbers and zero-based uptime on every record.

Telemetry, events, and session lifecycle records.

Previous-session inspection after reboot.

Detection of incomplete final lines.

Rejection of multiline JSON payloads.

No sequence consumption when validation or writing fails.

Configurable session-count, file-size, and total-storage limits.

Oldest complete sessions removed first.

Incomplete sessions preserved for investigation.

Storage health emitted as an observable state.

Proven on hardware

The ESP32 DOIT DevKit V1 example demonstrated:

SD mounting and repeated append operations.

More than one thousand telemetry writes in a continuous run.

Separate files across reset and power cycles.

Earlier sessions remaining readable after reboot.

Complete-final-line detection.

Invalid multiline payload rejection.

Sequence preservation after rejection.

Retention and health code compiling and running on-device.

Default retention policy

Maximum sessions:     100
Maximum session size: 2 MiB
Maximum total logs:   100 MiB

The active session is created only after retention runs. The oldest complete sessions are removed first when space must be reclaimed.

Record example

{"session":"EXAMPLE-000017","seq":0,"uptime_ms":612,"type":"session_start","data":{"boot_count":17,"firmware":"KSJ_Storage-example-0.2.0","board":"ESP32 DOIT DevKit V1"}}
{"session":"EXAMPLE-000017","seq":1,"uptime_ms":674,"type":"event","event":"EXAMPLE_READY","data":{"message":"Session logging works"}}
{"session":"EXAMPLE-000017","seq":4,"uptime_ms":5000,"type":"telemetry","data":{"simulated_raw":107,"stable":true}}

Compatibility

The v0.1.0 text-storage interface remains available. Existing users can continue using SDStorage for ordinary text files without adopting session logging.

Deferred

This release intentionally does not include Wi-Fi, RTC/NTP time, graphs, remote file browsing, a serial service console, or product-specific metrics.