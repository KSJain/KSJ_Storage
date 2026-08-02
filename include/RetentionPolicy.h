#pragma once

#include <Arduino.h>

namespace KSJ
{

struct RetentionPolicy
{
    uint16_t maximumSessions = 100;

    uint64_t maximumSessionBytes =
        2ULL * 1024ULL * 1024ULL;

    uint64_t maximumTotalBytes =
        100ULL * 1024ULL * 1024ULL;
};

}