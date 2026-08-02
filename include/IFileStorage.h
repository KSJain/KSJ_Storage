#pragma once

#include <Arduino.h>

#include "StorageResult.h"
#include "StorageStatus.h"

namespace KSJ
{

struct FileEntry
{
    String path;

    uint64_t sizeBytes = 0;

    bool isDirectory = false;
};

using FileEntryVisitor =
    bool (*)(
        const FileEntry& entry,
        void* context
    );

class IFileStorage
{
public:
    virtual ~IFileStorage() = default;

    virtual bool exists(
        const char* path
    ) const = 0;

    virtual StorageResult visitDirectory(
        const char* directoryPath,
        FileEntryVisitor visitor,
        void* context = nullptr
    ) = 0;
};

}