#include <Arduino.h>
#include <SPI.h>

#include "KSJ_SDStorage.h"
#include "RetentionPolicy.h"
#include "SessionInfo.h"
#include "SessionInspection.h"
#include "SessionInspector.h"
#include "SessionLogger.h"
#include "SessionRetention.h"
#include "SessionSequence.h"
#include "StorageHealth.h"

namespace Pins
{
    constexpr uint8_t SD_CS = 5;
    constexpr uint8_t SD_SCK = 18;
    constexpr uint8_t SD_MISO = 19;
    constexpr uint8_t SD_MOSI = 23;
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

KSJ::SessionSequence sessionSequence(
    storage
);

KSJ::SessionInspector sessionInspector(
    storage
);

KSJ::SessionRetention sessionRetention(
    storage
);

uint32_t previousTelemetryMs = 0;
uint32_t simulatedReading = 100;

String formatSessionNumber(
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

String makeSessionId(
    uint32_t number
)
{
    return
        String(
            Example::SESSION_PREFIX
        ) +
        formatSessionNumber(
            number
        );
}

String makeSessionPath(
    const String& sessionId
)
{
    return
        "/session_" +
        sessionId +
        ".jsonl";
}

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

void printInspection(
    const KSJ::SessionInspection& inspection
)
{
    Serial.println();
    Serial.println(
        "Previous session inspection"
    );

    Serial.println(
        "---------------------------"
    );

    Serial.print("Path:   ");
    Serial.println(inspection.path);

    Serial.print("Size:   ");
    Serial.print(
        static_cast<unsigned long>(
            inspection.sizeBytes
        )
    );
    Serial.println(" bytes");

    Serial.print("Status: ");
    Serial.println(
        KSJ::sessionInspectionStatusName(
            inspection.status
        )
    );

    Serial.println(
        "---------------------------"
    );
    Serial.println();
}

void printHealth(
    const KSJ::StorageHealth& health
)
{
    Serial.println();
    Serial.println(
        "Storage health"
    );

    Serial.println(
        "---------------------------"
    );

    Serial.print("State:      ");
    Serial.println(
        KSJ::storageHealthStateName(
            health.state
        )
    );

    Serial.print("Sessions:   ");
    Serial.println(
        health.sessionCount
    );

    Serial.print("Complete:   ");
    Serial.println(
        health.completeSessionCount
    );

    Serial.print("Incomplete: ");
    Serial.println(
        health.incompleteSessionCount
    );

    Serial.print("Deleted:    ");
    Serial.println(
        health.deletedSessionCount
    );

    Serial.print("Log bytes:  ");
    Serial.println(
        static_cast<unsigned long>(
            health.totalLogBytes
        )
    );

    Serial.print("Largest:    ");
    Serial.println(
        static_cast<unsigned long>(
            health.largestSessionBytes
        )
    );

    Serial.println(
        "---------------------------"
    );
    Serial.println();
}

String buildInspectionPayload(
    const String& previousSessionId,
    const KSJ::SessionInspection& inspection
)
{
    String payload;

    payload.reserve(192);

    payload +=
        "{\"previous_session\":\"";

    payload +=
        previousSessionId;

    payload +=
        "\",\"result\":\"";

    payload +=
        KSJ::sessionInspectionStatusName(
            inspection.status
        );

    payload +=
        "\",\"size_bytes\":";

    payload +=
        String(
            static_cast<unsigned long>(
                inspection.sizeBytes
            )
        );

    payload +=
        "}";

    return payload;
}

String buildHealthPayload(
    const KSJ::StorageHealth& health
)
{
    String payload;

    payload.reserve(224);

    payload += "{\"state\":\"";
    payload +=
        KSJ::storageHealthStateName(
            health.state
        );

    payload += "\",\"sessions\":";
    payload +=
        String(
            health.sessionCount
        );

    payload += ",\"complete\":";
    payload +=
        String(
            health.completeSessionCount
        );

    payload += ",\"incomplete\":";
    payload +=
        String(
            health.incompleteSessionCount
        );

    payload += ",\"deleted\":";
    payload +=
        String(
            health.deletedSessionCount
        );

    payload += ",\"total_bytes\":";
    payload +=
        String(
            static_cast<unsigned long>(
                health.totalLogBytes
            )
        );

    payload += "}";

    return payload;
}

void testInvalidPayload()
{
    const uint32_t beforeSequence =
        logger.sequence();

    const KSJ::StorageResult result =
        logger.logTelemetry(
            millis(),
            "{\n\"invalid\":true\n}"
        );

    const uint32_t afterSequence =
        logger.sequence();

    Serial.print(
        "Invalid payload rejected: "
    );

    Serial.println(
        !result
            ? "YES"
            : "NO"
    );

    Serial.print(
        "Sequence preserved: "
    );

    Serial.println(
        beforeSequence ==
                afterSequence
            ? "YES"
            : "NO"
    );
}

void setup()
{
    Serial.begin(115200);
    delay(400);

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

    KSJ::RetentionPolicy policy;

    policy.maximumSessions = 5;

    policy.maximumSessionBytes =
        2ULL * 1024ULL * 1024ULL;

    policy.maximumTotalBytes =
        100ULL * 1024ULL * 1024ULL;

    KSJ::StorageHealth health;

    const KSJ::StorageResult
        retentionResult =
            sessionRetention.apply(
                Example::SESSION_PREFIX,
                policy,
                health
            );

    printResult(
        "Retention",
        retentionResult
    );

    printHealth(
        health
    );

    if (!retentionResult)
    {
        Serial.println(
            "Application continues without retention."
        );
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

    KSJ::SessionInspection
        previousInspection;

    String previousSessionId;
    String previousSessionPath;

    const bool previousSessionExists =
        sessionNumber > 1;

    if (previousSessionExists)
    {
        previousSessionId =
            makeSessionId(
                sessionNumber - 1
            );

        previousSessionPath =
            makeSessionPath(
                previousSessionId
            );

        const KSJ::StorageResult
            inspectionResult =
                sessionInspector.inspect(
                    previousSessionPath.c_str(),
                    previousInspection
                );

        printResult(
            "Previous inspection",
            inspectionResult
        );

        printInspection(
            previousInspection
        );
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

    Serial.print("Session number: ");
    Serial.println(sessionNumber);

    Serial.print("Session ID: ");
    Serial.println(sessionId);

    Serial.print("Session file: ");
    Serial.println(
        logger.sessionPath()
    );

    logger.logEvent(
        millis(),
        "EXAMPLE_READY",
        "{\"message\":\"Session logging works\"}"
    );

    if (previousSessionExists)
    {
        const String payload =
            buildInspectionPayload(
                previousSessionId,
                previousInspection
            );

        logger.logEvent(
            millis(),
            "PREVIOUS_SESSION_INSPECTED",
            payload.c_str()
        );
    }

    const String healthPayload =
        buildHealthPayload(
            health
        );

    logger.logEvent(
        millis(),
        "STORAGE_HEALTH",
        healthPayload.c_str()
    );

    testInvalidPayload();
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

    simulatedReading += 7;

    String payload;

    payload.reserve(96);

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

    Serial.print("Telemetry ");

    Serial.print(
        result
            ? logger.sequence() - 1
            : logger.sequence()
    );

    Serial.print(": ");

    Serial.println(
        result
            ? "SAVED"
            : "FAILED"
    );
}