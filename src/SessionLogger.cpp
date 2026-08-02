#include "SessionLogger.h"

namespace KSJ
{

SessionLogger::SessionLogger(
    ITextStorage& storage
)
    : _storage(storage),
      _session(),
      _sessionPath(),
      _nextSequence(0),
      _ready(false),
      _sessionActive(false),
      _status(
          StorageStatus::NotInitialized
      )
{
}

StorageResult SessionLogger::begin(
    const SessionInfo& session
)
{
    _ready = false;
    _sessionActive = false;
    _nextSequence = 0;

    if (!_storage.isReady())
    {
        return makeResult(
            false,
            _storage.status()
        );
    }

    if (session.sessionId.length() == 0)
    {
        return makeResult(
            false,
            StorageStatus::WriteFailed
        );
    }

    _session = session;

    _sessionPath =
        "/session_" +
        session.sessionId +
        ".jsonl";

    SessionRecord startRecord;

    startRecord.type =
        SessionRecordType::SessionStart;

    startRecord.sequence =
        _nextSequence;

    startRecord.uptimeMs =
        session.startedAtMs;

    startRecord.payloadJson =
        buildSessionStartPayload();

    const StorageResult result =
        appendRecord(
            startRecord,
            true
        );

    if (!result)
    {
        return result;
    }

    advanceSequence();

    _ready = true;
    _sessionActive = true;

    return makeResult(
        true,
        StorageStatus::Ready
    );
}

StorageResult SessionLogger::logTelemetry(
    uint32_t uptimeMs,
    const char* payloadJson
)
{
    if (
        !_ready ||
        !_sessionActive
    )
    {
        return makeResult(
            false,
            StorageStatus::NotInitialized
        );
    }

    if (!validJsonObject(payloadJson))
    {
        return makeResult(
            false,
            StorageStatus::WriteFailed
        );
    }

    SessionRecord record;

    record.type =
        SessionRecordType::Telemetry;

    record.sequence =
        _nextSequence;

    record.uptimeMs =
        uptimeMs;

    record.payloadJson =
        payloadJson;

    const StorageResult result =
        appendRecord(
            record,
            false
        );

    if (result)
    {
        advanceSequence();
    }

    return result;
}

StorageResult SessionLogger::logEvent(
    uint32_t uptimeMs,
    const char* eventName,
    const char* payloadJson
)
{
    if (
        !_ready ||
        !_sessionActive
    )
    {
        return makeResult(
            false,
            StorageStatus::NotInitialized
        );
    }

    if (
        eventName == nullptr ||
        eventName[0] == '\0'
    )
    {
        return makeResult(
            false,
            StorageStatus::WriteFailed
        );
    }

    if (
        payloadJson != nullptr &&
        !validJsonObject(payloadJson)
    )
    {
        return makeResult(
            false,
            StorageStatus::WriteFailed
        );
    }

    SessionRecord record;

    record.type =
        SessionRecordType::Event;

    record.sequence =
        _nextSequence;

    record.uptimeMs =
        uptimeMs;

    record.eventName =
        eventName;

    record.payloadJson =
        payloadJson != nullptr
            ? payloadJson
            : "{}";

    const StorageResult result =
        appendRecord(
            record,
            false
        );

    if (result)
    {
        advanceSequence();
    }

    return result;
}

void SessionLogger::update(
    uint32_t nowMs
)
{
    (void)nowMs;
}

StorageResult SessionLogger::endSession(
    uint32_t uptimeMs
)
{
    if (
        !_ready ||
        !_sessionActive
    )
    {
        return makeResult(
            false,
            StorageStatus::NotInitialized
        );
    }

    SessionRecord record;

    record.type =
        SessionRecordType::SessionEnd;

    record.sequence =
        _nextSequence;

    record.uptimeMs =
        uptimeMs;

    record.payloadJson =
        "{}";

    const StorageResult result =
        appendRecord(
            record,
            false
        );

    if (result)
    {
        advanceSequence();
        _sessionActive = false;
    }

    return result;
}

bool SessionLogger::isReady() const
{
    return
        _ready &&
        _sessionActive;
}

StorageStatus SessionLogger::status() const
{
    return _status;
}

uint32_t SessionLogger::sequence() const
{
    return _nextSequence;
}

const char* SessionLogger::sessionPath() const
{
    return _sessionPath.c_str();
}

const SessionInfo&
SessionLogger::sessionInfo() const
{
    return _session;
}

StorageResult SessionLogger::appendRecord(
    const SessionRecord& record,
    bool createFile
)
{
    const String line =
        buildRecordJson(record) +
        "\n";

    const StorageResult result =
        createFile
            ? _storage.writeText(
                _sessionPath.c_str(),
                line.c_str()
            )
            : _storage.appendText(
                _sessionPath.c_str(),
                line.c_str()
            );

    if (!result)
    {
        _ready = false;

        return makeResult(
            false,
            result.status
        );
    }

    return makeResult(
        true,
        StorageStatus::Ready
    );
}

String SessionLogger::buildRecordJson(
    const SessionRecord& record
) const
{
    String json;

    json.reserve(
        256 +
        record.payloadJson.length()
    );

    json += "{\"session\":\"";
    json += escapeJsonString(_session.sessionId);
    json += "\",\"seq\":";
    json += String(record.sequence);
    json += ",\"uptime_ms\":";
    json += String(record.uptimeMs);
    json += ",\"type\":\"";
    json += sessionRecordTypeName(record.type);
    json += "\"";

    if (
        record.type ==
        SessionRecordType::Event
    )
    {
        json += ",\"event\":\"";
        json += escapeJsonString(record.eventName);
        json += "\"";
    }

    json += ",\"data\":";
    json += record.payloadJson;
    json += "}";

    return json;
}

String
SessionLogger::buildSessionStartPayload() const
{
    String payload;

    payload.reserve(192);

    payload += "{\"boot_count\":";
    payload += String(_session.bootCount);
    payload += ",\"firmware\":\"";
    payload += escapeJsonString(_session.firmwareVersion);
    payload += "\",\"board\":\"";
    payload += escapeJsonString(_session.boardName);
    payload += "\"}";

    return payload;
}

String SessionLogger::escapeJsonString(
    const String& value
) const
{
    String escaped;

    escaped.reserve(
        value.length() + 8
    );

    for (
        size_t index = 0;
        index < value.length();
        ++index
    )
    {
        const char character =
            value[index];

        switch (character)
        {
            case '\\':
                escaped += "\\\\";
                break;

            case '"':
                escaped += "\\\"";
                break;

            case '\n':
                escaped += "\\n";
                break;

            case '\r':
                escaped += "\\r";
                break;

            case '\t':
                escaped += "\\t";
                break;

            default:
                escaped += character;
                break;
        }
    }

    return escaped;
}

bool SessionLogger::validJsonObject(
    const char* json
) const
{
    if (json == nullptr)
    {
        return false;
    }

    /*
     * JSON Lines requires exactly one physical
     * record per line. Payloads containing literal
     * CR or LF characters must be rejected.
     */
    for (
        const char* cursor = json;
        *cursor != '\0';
        ++cursor
    )
    {
        if (
            *cursor == '\n' ||
            *cursor == '\r'
        )
        {
            return false;
        }
    }

    while (
        *json == ' ' ||
        *json == '\t'
    )
    {
        ++json;
    }

    if (*json != '{')
    {
        return false;
    }

    const char* end =
        json +
        strlen(json);

    while (
        end > json &&
        (
            end[-1] == ' ' ||
            end[-1] == '\t'
        )
    )
    {
        --end;
    }

    return
        end > json &&
        end[-1] == '}';
}

StorageResult SessionLogger::makeResult(
    bool success,
    StorageStatus status
)
{
    _status = status;

    StorageResult result;

    result.success = success;
    result.status = status;

    return result;
}

void SessionLogger::advanceSequence()
{
    if (_nextSequence < UINT32_MAX)
    {
        ++_nextSequence;
    }
}

}