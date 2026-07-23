#pragma once

#include "StorageStatus.h"

namespace KSJ
{

struct StorageResult
{
    bool success = false;

    StorageStatus status =
        StorageStatus::NotInitialized;

    explicit operator bool() const
    {
        return success;
    }
};

}