#pragma once

#include <Arduino.h>

namespace KSJ
{

enum class StorageCardType : uint8_t
{
    None,
    MMC,
    SDSC,
    SDHC,
    Unknown
};

struct StorageInfo
{
    StorageCardType cardType =
        StorageCardType::None;

    uint64_t cardSizeBytes = 0;
    uint64_t totalBytes = 0;
    uint64_t usedBytes = 0;

    bool valid = false;
};

}