#pragma once

#include <Arduino.h>

#include "IFileStorage.h"
#include "StorageResult.h"
#include "StorageStatus.h"

namespace KSJ
{

class SessionSequence
{
public:
    explicit SessionSequence(
        IFileStorage& storage
    );

    StorageResult findNext(
        const char* sessionPrefix,
        uint32_t& nextNumber,
        String& sessionId
    );

private:
    struct ScanContext
    {
        String expectedFilePrefix;

        String expectedFileSuffix;

        uint32_t highestNumber = 0;
    };

    static bool inspectEntry(
        const FileEntry& entry,
        void* context
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

    static String formatNumber(
        uint32_t number
    );

    IFileStorage& _storage;
};

}