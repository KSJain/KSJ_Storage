# KSJ_Storage

Reusable persistence for ESP32 projects.

KSJ_Storage provides a small, understandable interface for mounting SD cards and reading or writing text files.

Applications should think in terms of persistence—not filesystem driver details.

> Learn to walk before leap.

---

## Features

- SD-card mounting
- Card detection
- Card type reporting
- Capacity and usage information
- Text file writing
- Text file appending
- Text file reading
- Structured failure reporting
- Hardware-independent text-storage interface

---

## Current Release

**v0.1.0**

---

## Supported Storage

| Storage Type | Status |
|---|---|
| ESP32 SD over SPI | Supported |
| LittleFS | Planned |
| SPIFFS | Planned |
| External flash | Future |

---

## Wiring

Default example wiring for ESP32 DOIT DevKit V1:

| SD Module | ESP32 |
|---|---:|
| CS | GPIO 5 |
| SCK | GPIO 18 |
| MISO | GPIO 19 |
| MOSI | GPIO 23 |
| GND | GND |

Confirm whether the module requires 3.3 V or 5 V input before connecting power.

ESP32 signal logic is 3.3 V.

---

## Quick Start

```cpp
SPIClass sdSpi(VSPI);

KSJ::SDStorage storage(
    sdSpi,
    5
);

void setup()
{
    sdSpi.begin(
        18,
        19,
        23,
        5
    );

    if (!storage.begin())
    {
        return;
    }

    storage.writeText(
        "/hello.txt",
        "Hello from RAMU\n"
    );

    storage.appendText(
        "/hello.txt",
        "Persistent memory works.\n"
    );

    String contents;

    storage.readText(
        "/hello.txt",
        contents
    );
}