#pragma once

#include <Arduino.h>

#include "SessionInfo.h"
#include "StorageResult.h"
#include "StorageStatus.h"

namespace KSJ
{

class IStorageLogger
{
public:
    virtual ~IStorageLogger() = default;

    virtual StorageResult begin(
        const SessionInfo& session
    ) = 0;

    virtual StorageResult logTelemetry(
        uint32_t uptimeMs,
        const char* payloadJson
    ) = 0;

    virtual StorageResult logEvent(
        uint32_t uptimeMs,
        const char* eventName,
        const char* payloadJson = nullptr
    ) = 0;

    virtual void update(
        uint32_t nowMs
    ) = 0;

    virtual StorageResult endSession(
        uint32_t uptimeMs
    ) = 0;

    virtual bool isReady() const = 0;

    virtual StorageStatus status() const = 0;

    virtual uint32_t sequence() const = 0;

    virtual const char* sessionPath() const = 0;
};

}