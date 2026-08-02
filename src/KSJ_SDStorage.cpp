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

    File file =
        SD.open(
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

    const size_t expectedBytes =
        strlen(text);

    const size_t bytesWritten =
        file.print(text);

    file.flush();
    file.close();

    if (
        expectedBytes > 0 &&
        bytesWritten != expectedBytes
    )
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

    File file =
        SD.open(
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

    const size_t expectedBytes =
        strlen(text);

    const size_t bytesWritten =
        file.print(text);

    file.flush();
    file.close();

    if (
        expectedBytes > 0 &&
        bytesWritten != expectedBytes
    )
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

    File file =
        SD.open(
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

bool SDStorage::exists(
    const char* path
) const
{
    if (
        !_ready ||
        !validatePath(path)
    )
    {
        return false;
    }

    return SD.exists(path);
}

StorageResult SDStorage::visitDirectory(
    const char* directoryPath,
    FileEntryVisitor visitor,
    void* context
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
        directoryPath == nullptr ||
        directoryPath[0] != '/' ||
        visitor == nullptr
    )
    {
        return makeResult(
            false,
            StorageStatus::ReadFailed
        );
    }

    File directory =
        SD.open(
            directoryPath,
            FILE_READ
        );

    if (
        !directory ||
        !directory.isDirectory()
    )
    {
        if (directory)
        {
            directory.close();
        }

        return makeResult(
            false,
            StorageStatus::OpenFailed
        );
    }

    File entry =
        directory.openNextFile();

    while (entry)
    {
        FileEntry fileEntry;

        const char* entryName =
            entry.name();

        if (entryName != nullptr)
        {
            fileEntry.path =
                entryName;

            if (
                fileEntry.path.length() > 0 &&
                fileEntry.path[0] != '/'
            )
            {
                fileEntry.path =
                    "/" +
                    fileEntry.path;
            }
        }

        fileEntry.sizeBytes =
            entry.size();

        fileEntry.isDirectory =
            entry.isDirectory();

        const bool continueVisiting =
            visitor(
                fileEntry,
                context
            );

        entry.close();

        if (!continueVisiting)
        {
            break;
        }

        entry =
            directory.openNextFile();
    }

    directory.close();

    return makeResult(
        true,
        StorageStatus::Ready
    );
}

StorageResult SDStorage::fileSize(
    const char* path,
    uint64_t& sizeBytes
)
{
    sizeBytes = 0;

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

    File file =
        SD.open(
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

    if (file.isDirectory())
    {
        file.close();

        return makeResult(
            false,
            StorageStatus::ReadFailed
        );
    }

    sizeBytes =
        file.size();

    file.close();

    return makeResult(
        true,
        StorageStatus::Ready
    );
}

StorageResult SDStorage::readLastByte(
    const char* path,
    char& value
)
{
    value = '\0';

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

    File file =
        SD.open(
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

    if (
        file.isDirectory() ||
        file.size() == 0
    )
    {
        file.close();

        return makeResult(
            false,
            StorageStatus::ReadFailed
        );
    }

    const size_t finalPosition =
        file.size() - 1;

    const bool seekSucceeded =
        file.seek(
            finalPosition,
            SeekSet
        );

    if (!seekSucceeded)
    {
        file.close();

        return makeResult(
            false,
            StorageStatus::ReadFailed
        );
    }

    const int byteRead =
        file.read();

    file.close();

    if (byteRead < 0)
    {
        return makeResult(
            false,
            StorageStatus::ReadFailed
        );
    }

    value =
        static_cast<char>(
            byteRead
        );

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

    result.success =
        success;

    result.status =
        status;

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