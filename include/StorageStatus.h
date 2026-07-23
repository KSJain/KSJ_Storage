#pragma once

#include <Arduino.h>

namespace KSJ
{

enum class StorageStatus : uint8_t
{
    NotInitialized,
    Ready,
    MountFailed,
    CardNotFound,
    OpenFailed,
    WriteFailed,
    ReadFailed
};

}