#pragma once

#include <Arduino.h>

#include "StorageInfo.h"
#include "StorageResult.h"
#include "StorageStatus.h"

namespace KSJ
{

class ITextStorage
{
public:
    virtual ~ITextStorage() = default;

    virtual StorageResult begin() = 0;

    virtual bool isReady() const = 0;

    virtual StorageStatus status() const = 0;

    virtual StorageInfo info() const = 0;

    virtual StorageResult writeText(
        const char* path,
        const char* text
    ) = 0;

    virtual StorageResult appendText(
        const char* path,
        const char* text
    ) = 0;

    virtual StorageResult readText(
        const char* path,
        String& destination
    ) = 0;
};

}