#include "SessionInspector.h"

namespace KSJ
{

SessionInspector::SessionInspector(
    IFileStorage& storage
)
    : _storage(storage)
{
}

StorageResult SessionInspector::inspect(
    const char* path,
    SessionInspection& inspection
)
{
    inspection =
        SessionInspection{};

    if (
        path == nullptr ||
        path[0] == '\0'
    )
    {
        StorageResult result;

        result.success = false;
        result.status =
            StorageStatus::ReadFailed;

        inspection.status =
            SessionInspectionStatus::ReadFailed;

        return result;
    }

    inspection.path =
        path;

    if (!_storage.exists(path))
    {
        StorageResult result;

        result.success = true;
        result.status =
            StorageStatus::Ready;

        inspection.status =
            SessionInspectionStatus::Missing;

        return result;
    }

    uint64_t sizeBytes = 0;

    const StorageResult sizeResult =
        _storage.fileSize(
            path,
            sizeBytes
        );

    if (!sizeResult)
    {
        inspection.status =
            SessionInspectionStatus::ReadFailed;

        return sizeResult;
    }

    inspection.sizeBytes =
        sizeBytes;

    if (sizeBytes == 0)
    {
        inspection.status =
            SessionInspectionStatus::Empty;

        return sizeResult;
    }

    char finalByte = '\0';

    const StorageResult byteResult =
        _storage.readLastByte(
            path,
            finalByte
        );

    if (!byteResult)
    {
        inspection.status =
            SessionInspectionStatus::ReadFailed;

        return byteResult;
    }

    inspection.finalByte =
        finalByte;

    inspection.status =
        finalByte == '\n'
            ? SessionInspectionStatus::Complete
            : SessionInspectionStatus::
                IncompleteFinalLine;

    return byteResult;
}

}