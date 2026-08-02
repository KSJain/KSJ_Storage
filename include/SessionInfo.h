#pragma once

#include <Arduino.h>

namespace KSJ
{

struct SessionInfo
{
    String sessionId;

    uint32_t bootCount = 0;

    String firmwareVersion;

    String boardName;

    uint32_t startedAtMs = 0;
};

}