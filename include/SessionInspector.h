#pragma once

#include <Arduino.h>

#include "IFileStorage.h"
#include "SessionInspection.h"
#include "StorageResult.h"

namespace KSJ
{

class SessionInspector
{
public:
    explicit SessionInspector(
        IFileStorage& storage
    );

    StorageResult inspect(
        const char* path,
        SessionInspection& inspection
    );

private:
    IFileStorage& _storage;
};

}