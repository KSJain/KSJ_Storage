#pragma once

#include <Arduino.h>

namespace KSJ
{

enum class SessionInspectionStatus : uint8_t
{
    Complete,
    IncompleteFinalLine,
    Empty,
    Missing,
    ReadFailed
};

inline const char* sessionInspectionStatusName(
    SessionInspectionStatus status
)
{
    switch (status)
    {
        case SessionInspectionStatus::Complete:
            return "COMPLETE";

        case SessionInspectionStatus::IncompleteFinalLine:
            return "INCOMPLETE_FINAL_LINE";

        case SessionInspectionStatus::Empty:
            return "EMPTY";

        case SessionInspectionStatus::Missing:
            return "MISSING";

        case SessionInspectionStatus::ReadFailed:
        default:
            return "READ_FAILED";
    }
}

struct SessionInspection
{
    String path;

    uint64_t sizeBytes = 0;

    char finalByte = '\0';

    SessionInspectionStatus status =
        SessionInspectionStatus::Missing;

    bool isUsable() const
    {
        return
            status == SessionInspectionStatus::Complete ||
            status ==
                SessionInspectionStatus::IncompleteFinalLine;
    }

    bool hasIncompleteFinalLine() const
    {
        return
            status ==
            SessionInspectionStatus::IncompleteFinalLine;
    }
};

}