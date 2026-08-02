Changelog

All notable changes to KSJ_Storage are documented here.

[0.2.0] - 2026-08-02

Added

Append-only JSON Lines session logging.

Human-readable session identifiers.

Filename-derived session numbering.

Independent session files across resets and power cycles.

Session start, telemetry, event, and session end records.

Monotonic sequence numbers within each session.

Previous-session inspection after reboot.

Complete and incomplete-final-line detection.

Multiline payload rejection for JSON Lines integrity.

Sequence preservation when validation or writing fails.

Configurable retention policy.

Oldest-complete-session deletion when retention limits are exceeded.

Storage health reporting for session count, total log bytes, largest session, incomplete sessions, and deleted sessions.

Local PlatformIO library-development workflow documentation.

Basic session logger hardware example.

Changed

SDStorage now supports directory iteration, file sizing, final-byte inspection, existence checks, and file removal.

File writes and appends verify the expected byte count.

Files are flushed and closed after each record for conservative power-loss resilience.

Public library metadata now exposes the v0.2.0 session, inspection, retention, and health APIs.

Preserved

Existing v0.1.0 text-storage APIs remain available.

Storage remains optional to the application.

A storage failure does not become permission to stop sensing, UI, input, or safe control behavior.

Engineering intent

Storage is an observer.

Observation before optimization.

Every observation should be useful more than once.

[0.1.0]

Added

Reusable SD text-storage abstraction.

SD-card mounting and card information.

Text write, append, and read operations.

Structured storage results and status reporting.