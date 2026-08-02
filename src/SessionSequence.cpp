#include "SessionSequence.h"

namespace KSJ
{

SessionSequence::SessionSequence(
    IFileStorage& storage
)
    : _storage(storage)
{
}

StorageResult SessionSequence::findNext(
    const char* sessionPrefix,
    uint32_t& nextNumber,
    String& sessionId
)
{
    nextNumber = 0;
    sessionId = "";

    StorageResult result;

    if (
        sessionPrefix == nullptr ||
        sessionPrefix[0] == '\0'
    )
    {
        result.success = false;
        result.status =
            StorageStatus::ReadFailed;

        return result;
    }

    ScanContext context;

    context.expectedFilePrefix =
        "session_";

    context.expectedFilePrefix +=
        sessionPrefix;

    context.expectedFileSuffix =
        ".jsonl";

    const StorageResult scanResult =
        _storage.visitDirectory(
            "/",
            inspectEntry,
            &context
        );

    if (!scanResult)
    {
        return scanResult;
    }

    /*
     * Overflow is unrealistic for this device,
     * but avoid wrapping silently.
     */
    if (
        context.highestNumber ==
        UINT32_MAX
    )
    {
        result.success = false;
        result.status =
            StorageStatus::ReadFailed;

        return result;
    }

    nextNumber =
        context.highestNumber + 1;

    sessionId =
        sessionPrefix;

    sessionId +=
        formatNumber(
            nextNumber
        );

    result.success = true;
    result.status =
        StorageStatus::Ready;

    return result;
}

bool SessionSequence::inspectEntry(
    const FileEntry& entry,
    void* context
)
{
    if (
        context == nullptr ||
        entry.isDirectory
    )
    {
        return true;
    }

    ScanContext* scanContext =
        static_cast<ScanContext*>(
            context
        );

    const String fileName =
        baseName(
            entry.path
        );

    uint32_t parsedNumber = 0;

    if (
        parseSessionNumber(
            fileName,
            scanContext->
                expectedFilePrefix,
            scanContext->
                expectedFileSuffix,
            parsedNumber
        ) &&
        parsedNumber >
            scanContext->
                highestNumber
    )
    {
        scanContext->
            highestNumber =
                parsedNumber;
    }

    return true;
}

String SessionSequence::baseName(
    const String& path
)
{
    const int lastSlash =
        path.lastIndexOf('/');

    if (lastSlash < 0)
    {
        return path;
    }

    return path.substring(
        lastSlash + 1
    );
}

bool SessionSequence::parseSessionNumber(
    const String& fileName,
    const String& prefix,
    const String& suffix,
    uint32_t& number
)
{
    number = 0;

    if (
        !fileName.startsWith(prefix) ||
        !fileName.endsWith(suffix)
    )
    {
        return false;
    }

    const int numberStart =
        prefix.length();

    const int numberEnd =
        fileName.length() -
        suffix.length();

    if (numberEnd <= numberStart)
    {
        return false;
    }

    const String numberText =
        fileName.substring(
            numberStart,
            numberEnd
        );

    if (numberText.length() == 0)
    {
        return false;
    }

    uint64_t parsed = 0;

    for (
        size_t index = 0;
        index < numberText.length();
        ++index
    )
    {
        const char character =
            numberText[index];

        if (
            character < '0' ||
            character > '9'
        )
        {
            return false;
        }

        parsed =
            parsed * 10ULL +
            static_cast<uint64_t>(
                character - '0'
            );

        if (parsed > UINT32_MAX)
        {
            return false;
        }
    }

    number =
        static_cast<uint32_t>(
            parsed
        );

    return true;
}

String SessionSequence::formatNumber(
    uint32_t number
)
{
    String formatted =
        String(number);

    while (formatted.length() < 6)
    {
        formatted =
            "0" +
            formatted;
    }

    return formatted;
}

}