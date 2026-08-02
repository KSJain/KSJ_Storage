#pragma once

#include <Arduino.h>

namespace KSJ
{

enum class SessionRecordType : uint8_t
{
    SessionStart,
    Telemetry,
    Event,
    SessionEnd
};

inline const char* sessionRecordTypeName(
    SessionRecordType type
)
{
    switch (type)
    {
        case SessionRecordType::SessionStart:
            return "session_start";

        case SessionRecordType::Telemetry:
            return "telemetry";

        case SessionRecordType::Event:
            return "event";

        case SessionRecordType::SessionEnd:
            return "session_end";

        default:
            return "unknown";
    }
}

struct SessionRecord
{
    SessionRecordType type =
        SessionRecordType::Telemetry;

    uint32_t sequence = 0;

    uint32_t uptimeMs = 0;

    String eventName;

    /*
     * A complete JSON object supplied by the
     * application.
     *
     * Example:
     *
     * {"ldr_raw":2610,"stable":true}
     */
    String payloadJson;
};

}