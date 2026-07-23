#include "KSJ_SDStorage.h"

namespace KSJ
{

SDStorage::SDStorage(
    SPIClass& spi,
    uint8_t chipSelectPin,
    uint32_t frequencyHz
)
    : _spi(spi),
      _chipSelectPin(chipSelectPin),
      _frequencyHz(frequencyHz),
      _ready(false),
      _status(StorageStatus::NotInitialized)
{
}

StorageResult SDStorage::begin()
{
    _ready = false;

    const bool mounted =
        SD.begin(
            _chipSelectPin,
            _spi,
            _frequencyHz
        );

    if (!mounted)
    {
        return makeResult(
            false,
            StorageStatus::MountFailed
        );
    }

    if (SD.cardType() == CARD_NONE)
    {
        return makeResult(
            false,
            StorageStatus::CardNotFound
        );
    }

    _ready = true;

    return makeResult(
        true,
        StorageStatus::Ready
    );
}

bool SDStorage::isReady() const
{
    return _ready;
}

StorageStatus SDStorage::status() const
{
    return _status;
}

StorageInfo SDStorage::info() const
{
    StorageInfo storageInfo;

    if (!_ready)
    {
        return storageInfo;
    }

    storageInfo.cardType =
        detectCardType();

    storageInfo.cardSizeBytes =
        SD.cardSize();

    storageInfo.totalBytes =
        SD.totalBytes();

    storageInfo.usedBytes =
        SD.usedBytes();

    storageInfo.valid =
        storageInfo.cardType !=
        StorageCardType::None;

    return storageInfo;
}

StorageResult SDStorage::writeText(
    const char* path,
    const char* text
)
{
    if (!_ready)
    {
        return makeResult(
            false,
            StorageStatus::NotInitialized
        );
    }

    if (
        !validatePath(path) ||
        text == nullptr
    )
    {
        return makeResult(
            false,
            StorageStatus::WriteFailed
        );
    }

    File file = SD.open(
        path,
        FILE_WRITE
    );

    if (!file)
    {
        return makeResult(
            false,
            StorageStatus::OpenFailed
        );
    }

    const size_t bytesWritten =
        file.print(text);

    file.close();

    if (bytesWritten == 0)
    {
        return makeResult(
            false,
            StorageStatus::WriteFailed
        );
    }

    return makeResult(
        true,
        StorageStatus::Ready
    );
}

StorageResult SDStorage::appendText(
    const char* path,
    const char* text
)
{
    if (!_ready)
    {
        return makeResult(
            false,
            StorageStatus::NotInitialized
        );
    }

    if (
        !validatePath(path) ||
        text == nullptr
    )
    {
        return makeResult(
            false,
            StorageStatus::WriteFailed
        );
    }

    File file = SD.open(
        path,
        FILE_APPEND
    );

    if (!file)
    {
        return makeResult(
            false,
            StorageStatus::OpenFailed
        );
    }

    const size_t bytesWritten =
        file.print(text);

    file.close();

    if (bytesWritten == 0)
    {
        return makeResult(
            false,
            StorageStatus::WriteFailed
        );
    }

    return makeResult(
        true,
        StorageStatus::Ready
    );
}

StorageResult SDStorage::readText(
    const char* path,
    String& destination
)
{
    destination = "";

    if (!_ready)
    {
        return makeResult(
            false,
            StorageStatus::NotInitialized
        );
    }

    if (!validatePath(path))
    {
        return makeResult(
            false,
            StorageStatus::ReadFailed
        );
    }

    File file = SD.open(
        path,
        FILE_READ
    );

    if (!file)
    {
        return makeResult(
            false,
            StorageStatus::OpenFailed
        );
    }

    while (file.available())
    {
        destination +=
            static_cast<char>(
                file.read()
            );
    }

    file.close();

    return makeResult(
        true,
        StorageStatus::Ready
    );
}

uint8_t SDStorage::chipSelectPin() const
{
    return _chipSelectPin;
}

uint32_t SDStorage::frequencyHz() const
{
    return _frequencyHz;
}

StorageResult SDStorage::makeResult(
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

StorageCardType
SDStorage::detectCardType() const
{
    switch (SD.cardType())
    {
        case CARD_MMC:
            return StorageCardType::MMC;

        case CARD_SD:
            return StorageCardType::SDSC;

        case CARD_SDHC:
            return StorageCardType::SDHC;

        case CARD_NONE:
            return StorageCardType::None;

        default:
            return StorageCardType::Unknown;
    }
}

bool SDStorage::validatePath(
    const char* path
) const
{
    return
        path != nullptr &&
        path[0] == '/' &&
        path[1] != '\0';
}

}