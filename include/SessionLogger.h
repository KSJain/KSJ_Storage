#pragma once

#include <Arduino.h>

#include "IStorageLogger.h"
#include "ITextStorage.h"
#include "SessionInfo.h"
#include "SessionRecord.h"
#include "StorageResult.h"
#include "StorageStatus.h"

namespace KSJ
{

class SessionLogger final
    : public IStorageLogger
{
public:
    explicit SessionLogger(
        ITextStorage& storage
    );

    StorageResult begin(
        const SessionInfo& session
    ) override;

    StorageResult logTelemetry(
        uint32_t uptimeMs,
        const char* payloadJson
    ) override;

    StorageResult logEvent(
        uint32_t uptimeMs,
        const char* eventName,
        const char* payloadJson = nullptr
    ) override;

    void update(
        uint32_t nowMs
    ) override;

    StorageResult endSession(
        uint32_t uptimeMs
    ) override;

    bool isReady() const override;

    StorageStatus status() const override;

    uint32_t sequence() const override;

    const char* sessionPath() const override;

    const SessionInfo& sessionInfo() const;

private:
    StorageResult appendRecord(
        const SessionRecord& record,
        bool createFile
    );

    String buildRecordJson(
        const SessionRecord& record
    ) const;

    String buildSessionStartPayload() const;

    String escapeJsonString(
        const String& value
    ) const;

    bool validJsonObject(
        const char* json
    ) const;

    StorageResult makeResult(
        bool success,
        StorageStatus status
    );

    void advanceSequence();

    ITextStorage& _storage;

    SessionInfo _session;

    String _sessionPath;

    uint32_t _nextSequence;

    bool _ready;
    bool _sessionActive;

    StorageStatus _status;
};

}