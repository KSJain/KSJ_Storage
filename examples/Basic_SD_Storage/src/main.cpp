#include <Arduino.h>
#include <SPI.h>

#include <KSJ_SDStorage.h>
#include <StorageInfo.h>
#include <StorageResult.h>
#include <StorageStatus.h>

namespace Pins
{
    constexpr uint8_t SD_CS = 5;
    constexpr uint8_t SD_SCK = 18;
    constexpr uint8_t SD_MISO = 19;
    constexpr uint8_t SD_MOSI = 23;
}

namespace StorageConfig
{
    constexpr uint32_t SPI_FREQUENCY_HZ =
        4'000'000;

    constexpr char TEST_FILE[] =
        "/ksj_storage_test.txt";
}

SPIClass sdSpi(VSPI);

KSJ::SDStorage storage(
    sdSpi,
    Pins::SD_CS,
    StorageConfig::SPI_FREQUENCY_HZ
);

const char* statusName(
    KSJ::StorageStatus status
)
{
    switch (status)
    {
        case KSJ::StorageStatus::NotInitialized:
            return "NOT INITIALIZED";

        case KSJ::StorageStatus::Ready:
            return "READY";

        case KSJ::StorageStatus::MountFailed:
            return "MOUNT FAILED";

        case KSJ::StorageStatus::CardNotFound:
            return "CARD NOT FOUND";

        case KSJ::StorageStatus::OpenFailed:
            return "OPEN FAILED";

        case KSJ::StorageStatus::WriteFailed:
            return "WRITE FAILED";

        case KSJ::StorageStatus::ReadFailed:
            return "READ FAILED";

        default:
            return "UNKNOWN";
    }
}

const char* cardTypeName(
    KSJ::StorageCardType cardType
)
{
    switch (cardType)
    {
        case KSJ::StorageCardType::MMC:
            return "MMC";

        case KSJ::StorageCardType::SDSC:
            return "SDSC";

        case KSJ::StorageCardType::SDHC:
            return "SDHC/SDXC";

        case KSJ::StorageCardType::None:
            return "NONE";

        case KSJ::StorageCardType::Unknown:
        default:
            return "UNKNOWN";
    }
}

void printResult(
    const char* operation,
    const KSJ::StorageResult& result
)
{
    Serial.print(operation);
    Serial.print(": ");

    if (result.success)
    {
        Serial.println("SUCCESS");
        return;
    }

    Serial.print("FAILED - ");
    Serial.println(
        statusName(result.status)
    );
}

void printStorageInfo()
{
    const KSJ::StorageInfo info =
        storage.info();

    if (!info.valid)
    {
        Serial.println(
            "Storage information unavailable."
        );

        return;
    }

    constexpr uint64_t BYTES_PER_MB =
        1024ULL * 1024ULL;

    Serial.print("Card type: ");
    Serial.println(
        cardTypeName(info.cardType)
    );

    Serial.print("Card size: ");
    Serial.print(
        info.cardSizeBytes /
        BYTES_PER_MB
    );
    Serial.println(" MB");

    Serial.print("Filesystem size: ");
    Serial.print(
        info.totalBytes /
        BYTES_PER_MB
    );
    Serial.println(" MB");

    Serial.print("Used space: ");
    Serial.print(
        info.usedBytes /
        BYTES_PER_MB
    );
    Serial.println(" MB");
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println(
        "KSJ_Storage Basic_SD_Storage"
    );

    sdSpi.begin(
        Pins::SD_SCK,
        Pins::SD_MISO,
        Pins::SD_MOSI,
        Pins::SD_CS
    );

    const KSJ::StorageResult mountResult =
        storage.begin();

    printResult(
        "Mount",
        mountResult
    );

    if (!mountResult.success)
    {
        Serial.println(
            "Check card, wiring, power, format, and CS pin."
        );

        return;
    }

    printStorageInfo();

    const KSJ::StorageResult writeResult =
        storage.writeText(
            StorageConfig::TEST_FILE,
            "KSJain Storage Library Test\n"
            "Initial write succeeded.\n"
        );

    printResult(
        "Write",
        writeResult
    );

    String appendLine;

    appendLine =
        "Appended at uptime: ";

    appendLine +=
        String(millis());

    appendLine +=
        " ms\n";

    const KSJ::StorageResult appendResult =
        storage.appendText(
            StorageConfig::TEST_FILE,
            appendLine.c_str()
        );

    printResult(
        "Append",
        appendResult
    );

    String contents;

    const KSJ::StorageResult readResult =
        storage.readText(
            StorageConfig::TEST_FILE,
            contents
        );

    printResult(
        "Read",
        readResult
    );

    if (readResult.success)
    {
        Serial.println();
        Serial.println("File contents");
        Serial.println(
            "------------------------"
        );

        Serial.print(contents);

        Serial.println(
            "------------------------"
        );
    }

    Serial.println();
    Serial.println(
        "KSJ_Storage test complete."
    );
}

void loop()
{
}