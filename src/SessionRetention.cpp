#include "SessionRetention.h"

namespace KSJ
{

SessionRetention::SessionRetention(
    IFileStorage& storage
)
    : _storage(storage)
{
}

StorageResult SessionRetention::apply(
    const char* sessionPrefix,
    const RetentionPolicy& policy,
    StorageHealth& health
)
{
    health = StorageHealth{};
    _entryCount = 0;

    StorageResult result;

    if (
        sessionPrefix == nullptr ||
        sessionPrefix[0] == '\0'
    )
    {
        result.success = false;
        result.status =
            StorageStatus::ReadFailed;

        health.state =
            StorageHealthState::Error;

        return result;
    }

    ScanContext context;

    context.owner = this;

    context.expectedPrefix =
        "session_";

    context.expectedPrefix +=
        sessionPrefix;

    context.expectedSuffix =
        ".jsonl";

    const StorageResult scanResult =
        _storage.visitDirectory(
            "/",
            inspectEntry,
            &context
        );

    if (!scanResult)
    {
        health.state =
            StorageHealthState::Unavailable;

        return scanResult;
    }

    calculateHealth(
        policy,
        health
    );

    while (
        (
            _entryCount >=
                policy.maximumSessions ||
            health.totalLogBytes >
                policy.maximumTotalBytes
        )
    )
    {
        const int oldestIndex =
            findOldestCompleteIndex();

        if (oldestIndex < 0)
        {
            break;
        }

        const StorageResult removeResult =
            _storage.remove(
                _entries[oldestIndex]
                    .path.c_str()
            );

        if (!removeResult)
        {
            health.state =
                StorageHealthState::Error;

            return removeResult;
        }

        ++health.deletedSessionCount;

        removeEntryAt(
            static_cast<uint16_t>(
                oldestIndex
            )
        );

        calculateHealth(
            policy,
            health
        );

        health.retentionApplied = true;
    }

    calculateHealth(
        policy,
        health
    );

    health.retentionApplied =
        health.retentionApplied ||
        health.deletedSessionCount > 0;

    result.success = true;
    result.status =
        StorageStatus::Ready;

    return result;
}

bool SessionRetention::inspectEntry(
    const FileEntry& entry,
    void* context
)
{
    if (
        context == nullptr ||
        entry.isDirectory
    )
    {
        return true;
    }

    ScanContext* scanContext =
        static_cast<ScanContext*>(
            context
        );

    if (scanContext->owner == nullptr)
    {
        return false;
    }

    return scanContext->owner->addEntry(
        entry,
        scanContext->expectedPrefix,
        scanContext->expectedSuffix
    );
}

bool SessionRetention::addEntry(
    const FileEntry& entry,
    const String& expectedPrefix,
    const String& expectedSuffix
)
{
    if (
        _entryCount >=
        MAX_TRACKED_SESSIONS
    )
    {
        return false;
    }

    const String fileName =
        baseName(
            entry.path
        );

    uint32_t number = 0;

    if (
        !parseSessionNumber(
            fileName,
            expectedPrefix,
            expectedSuffix,
            number
        )
    )
    {
        return true;
    }

    SessionEntry& session =
        _entries[_entryCount];

    session.path =
        entry.path;

    session.number =
        number;

    session.sizeBytes =
        entry.sizeBytes;

    char finalByte = '\0';

    const StorageResult byteResult =
        _storage.readLastByte(
            entry.path.c_str(),
            finalByte
        );

    session.complete =
        byteResult &&
        finalByte == '\n';

    ++_entryCount;

    return true;
}

String SessionRetention::baseName(
    const String& path
)
{
    const int lastSlash =
        path.lastIndexOf('/');

    if (lastSlash < 0)
    {
        return path;
    }

    return path.substring(
        lastSlash + 1
    );
}

bool SessionRetention::parseSessionNumber(
    const String& fileName,
    const String& prefix,
    const String& suffix,
    uint32_t& number
)
{
    number = 0;

    if (
        !fileName.startsWith(prefix) ||
        !fileName.endsWith(suffix)
    )
    {
        return false;
    }

    const int numberStart =
        prefix.length();

    const int numberEnd =
        fileName.length() -
        suffix.length();

    if (numberEnd <= numberStart)
    {
        return false;
    }

    const String numberText =
        fileName.substring(
            numberStart,
            numberEnd
        );

    uint64_t parsed = 0;

    for (
        size_t index = 0;
        index < numberText.length();
        ++index
    )
    {
        const char character =
            numberText[index];

        if (
            character < '0' ||
            character > '9'
        )
        {
            return false;
        }

        parsed =
            parsed * 10ULL +
            static_cast<uint64_t>(
                character - '0'
            );

        if (parsed > UINT32_MAX)
        {
            return false;
        }
    }

    number =
        static_cast<uint32_t>(
            parsed
        );

    return true;
}

int SessionRetention::
findOldestCompleteIndex() const
{
    int oldestIndex = -1;
    uint32_t oldestNumber =
        UINT32_MAX;

    for (
        uint16_t index = 0;
        index < _entryCount;
        ++index
    )
    {
        const SessionEntry& entry =
            _entries[index];

        if (
            entry.complete &&
            entry.number < oldestNumber
        )
        {
            oldestNumber =
                entry.number;

            oldestIndex =
                static_cast<int>(
                    index
                );
        }
    }

    return oldestIndex;
}

void SessionRetention::removeEntryAt(
    uint16_t index
)
{
    if (index >= _entryCount)
    {
        return;
    }

    for (
        uint16_t current = index;
        current + 1 < _entryCount;
        ++current
    )
    {
        _entries[current] =
            _entries[current + 1];
    }

    --_entryCount;
}

void SessionRetention::calculateHealth(
    const RetentionPolicy& policy,
    StorageHealth& health
) const
{
    health.sessionCount =
        _entryCount;

    health.completeSessionCount = 0;
    health.incompleteSessionCount = 0;
    health.totalLogBytes = 0;
    health.largestSessionBytes = 0;

    for (
        uint16_t index = 0;
        index < _entryCount;
        ++index
    )
    {
        const SessionEntry& entry =
            _entries[index];

        health.totalLogBytes +=
            entry.sizeBytes;

        if (
            entry.sizeBytes >
            health.largestSessionBytes
        )
        {
            health.largestSessionBytes =
                entry.sizeBytes;
        }

        if (entry.complete)
        {
            ++health.completeSessionCount;
        }
        else
        {
            ++health.incompleteSessionCount;
        }
    }

    if (
        health.sessionCount >=
            policy.maximumSessions ||
        health.totalLogBytes >=
            policy.maximumTotalBytes ||
        health.largestSessionBytes >=
            policy.maximumSessionBytes
    )
    {
        health.state =
            StorageHealthState::
                CapacityReached;

        return;
    }

    const bool sessionsNearLimit =
        health.sessionCount >=
        static_cast<uint16_t>(
            policy.maximumSessions *
            8U / 10U
        );

    const bool totalNearLimit =
        health.totalLogBytes >=
        policy.maximumTotalBytes *
        8ULL / 10ULL;

    const bool fileNearLimit =
        health.largestSessionBytes >=
        policy.maximumSessionBytes *
        8ULL / 10ULL;

    health.state =
        (
            sessionsNearLimit ||
            totalNearLimit ||
            fileNearLimit
        )
            ? StorageHealthState::
                NearLimit
            : StorageHealthState::
                Healthy;
}

}