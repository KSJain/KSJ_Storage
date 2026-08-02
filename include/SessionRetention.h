#pragma once

#include <Arduino.h>

#include "IFileStorage.h"
#include "RetentionPolicy.h"
#include "StorageHealth.h"
#include "StorageResult.h"

namespace KSJ
{

class SessionRetention
{
public:
    explicit SessionRetention(
        IFileStorage& storage
    );

    StorageResult apply(
        const char* sessionPrefix,
        const RetentionPolicy& policy,
        StorageHealth& health
    );

private:
    static constexpr uint16_t
        MAX_TRACKED_SESSIONS = 128;

    struct SessionEntry
    {
        String path;
        uint32_t number = 0;
        uint64_t sizeBytes = 0;
        bool complete = false;
    };

    struct ScanContext
    {
        SessionRetention* owner = nullptr;
        String expectedPrefix;
        String expectedSuffix;
    };

    static bool inspectEntry(
        const FileEntry& entry,
        void* context
    );

    bool addEntry(
        const FileEntry& entry,
        const String& expectedPrefix,
        const String& expectedSuffix
    );

    static String baseName(
        const String& path
    );

    static bool parseSessionNumber(
        const String& fileName,
        const String& prefix,
        const String& suffix,
        uint32_t& number
    );

    int findOldestCompleteIndex() const;

    void removeEntryAt(
        uint16_t index
    );

    void calculateHealth(
        const RetentionPolicy& policy,
        StorageHealth& health
    ) const;

    IFileStorage& _storage;

    SessionEntry
        _entries[MAX_TRACKED_SESSIONS];

    uint16_t _entryCount = 0;
};

}