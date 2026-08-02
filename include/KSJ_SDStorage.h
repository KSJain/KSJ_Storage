#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "IFileStorage.h"
#include "ITextStorage.h"
#include "StorageInfo.h"
#include "StorageResult.h"
#include "StorageStatus.h"

namespace KSJ
{

class SDStorage final
    : public ITextStorage,
      public IFileStorage
{
public:
    SDStorage(
        SPIClass& spi,
        uint8_t chipSelectPin,
        uint32_t frequencyHz = 4000000
    );

    StorageResult begin() override;

    bool isReady() const override;

    StorageStatus status() const override;

    StorageInfo info() const override;

    StorageResult writeText(
        const char* path,
        const char* text
    ) override;

    StorageResult appendText(
        const char* path,
        const char* text
    ) override;

    StorageResult readText(
        const char* path,
        String& destination
    ) override;

    bool exists(
        const char* path
    ) const override;

    StorageResult visitDirectory(
        const char* directoryPath,
        FileEntryVisitor visitor,
        void* context = nullptr
    ) override;

    StorageResult fileSize(
        const char* path,
        uint64_t& sizeBytes
    ) override;

    StorageResult readLastByte(
        const char* path,
        char& value
    ) override;

    StorageResult remove(
        const char* path
    ) override;

    uint8_t chipSelectPin() const;

    uint32_t frequencyHz() const;

private:
    StorageResult makeResult(
        bool success,
        StorageStatus status
    );

    StorageCardType detectCardType() const;

    bool validatePath(
        const char* path
    ) const;

    SPIClass& _spi;

    uint8_t _chipSelectPin;
    uint32_t _frequencyHz;

    bool _ready;

    StorageStatus _status;
};

}