#include <Arduino.h>
#include <SPI.h>

#include "KSJ_SDStorage.h"
#include "SessionInfo.h"
#include "SessionLogger.h"
#include "SessionSequence.h"

namespace Pins
{
    constexpr uint8_t SD_CS =
        5;

    constexpr uint8_t SD_SCK =
        18;

    constexpr uint8_t SD_MISO =
        19;

    constexpr uint8_t SD_MOSI =
        23;
}

namespace Example
{
    constexpr char SESSION_PREFIX[] =
        "EXAMPLE-";

    constexpr char FIRMWARE[] =
        "KSJ_Storage-example-0.2.0";

    constexpr char BOARD[] =
        "ESP32 DOIT DevKit V1";

    constexpr uint32_t
        TELEMETRY_INTERVAL_MS =
            5000;
}

SPIClass sdSpi(
    VSPI
);

KSJ::SDStorage storage(
    sdSpi,
    Pins::SD_CS
);

KSJ::SessionLogger logger(
    storage
);

KSJ::SessionSequence
    sessionSequence(
        storage
    );

uint32_t previousTelemetryMs =
    0;

uint32_t simulatedReading =
    100;

void printResult(
    const char* operation,
    const KSJ::StorageResult& result
)
{
    Serial.print(operation);
    Serial.print(": ");

    Serial.println(
        result
            ? "SUCCESS"
            : "FAILED"
    );
}

void setup()
{
    Serial.begin(
        115200
    );

    delay(
        400
    );

    Serial.println();

    Serial.println(
        "KSJ Storage Session Logger"
    );

    sdSpi.begin(
        Pins::SD_SCK,
        Pins::SD_MISO,
        Pins::SD_MOSI,
        Pins::SD_CS
    );

    const KSJ::StorageResult
        mountResult =
            storage.begin();

    printResult(
        "Storage mount",
        mountResult
    );

    if (!mountResult)
    {
        Serial.println(
            "Application continues without logging."
        );

        return;
    }

    uint32_t sessionNumber = 0;

    String sessionId;

    const KSJ::StorageResult
        sequenceResult =
            sessionSequence.findNext(
                Example::SESSION_PREFIX,
                sessionNumber,
                sessionId
            );

    printResult(
        "Session sequence",
        sequenceResult
    );

    if (!sequenceResult)
    {
        return;
    }

    KSJ::SessionInfo session;

    session.sessionId =
        sessionId;

    session.bootCount =
        sessionNumber;

    session.firmwareVersion =
        Example::FIRMWARE;

    session.boardName =
        Example::BOARD;

    session.startedAtMs =
        millis();

    const KSJ::StorageResult
        sessionResult =
            logger.begin(
                session
            );

    printResult(
        "Session start",
        sessionResult
    );

    if (!sessionResult)
    {
        return;
    }

    Serial.print(
        "Session number: "
    );

    Serial.println(
        sessionNumber
    );

    Serial.print(
        "Session ID: "
    );

    Serial.println(
        sessionId
    );

    Serial.print(
        "Session file: "
    );

    Serial.println(
        logger.sessionPath()
    );

    logger.logEvent(
        millis(),
        "EXAMPLE_READY",
        "{\"message\":\"Session logging works\"}"
    );
}

void loop()
{
    const uint32_t nowMs =
        millis();

    logger.update(
        nowMs
    );

    if (
        !logger.isReady() ||
        nowMs -
            previousTelemetryMs <
        Example::
            TELEMETRY_INTERVAL_MS
    )
    {
        return;
    }

    previousTelemetryMs =
        nowMs;

    simulatedReading +=
        7;

    String payload;

    payload.reserve(
        96
    );

    payload +=
        "{\"simulated_raw\":";

    payload +=
        String(
            simulatedReading
        );

    payload +=
        ",\"stable\":true}";

    const KSJ::StorageResult result =
        logger.logTelemetry(
            nowMs,
            payload.c_str()
        );

    Serial.print(
        "Telemetry "
    );

    Serial.print(
        logger.sequence() - 1
    );

    Serial.print(
        ": "
    );

    Serial.println(
        result
            ? "SAVED"
            : "FAILED"
    );
}