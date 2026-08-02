#pragma once

#include <Arduino.h>

namespace KSJ
{

enum class StorageHealthState : uint8_t
{
    Healthy,
    NearLimit,
    CapacityReached,
    Unavailable,
    Error
};

inline const char* storageHealthStateName(
    StorageHealthState state
)
{
    switch (state)
    {
        case StorageHealthState::Healthy:
            return "HEALTHY";

        case StorageHealthState::NearLimit:
            return "NEAR_LIMIT";

        case StorageHealthState::CapacityReached:
            return "CAPACITY_REACHED";

        case StorageHealthState::Unavailable:
            return "UNAVAILABLE";

        case StorageHealthState::Error:
        default:
            return "ERROR";
    }
}

struct StorageHealth
{
    StorageHealthState state =
        StorageHealthState::Unavailable;

    uint16_t sessionCount = 0;

    uint16_t completeSessionCount = 0;

    uint16_t incompleteSessionCount = 0;

    uint16_t deletedSessionCount = 0;

    uint64_t totalLogBytes = 0;

    uint64_t largestSessionBytes = 0;

    bool retentionApplied = false;
};

}