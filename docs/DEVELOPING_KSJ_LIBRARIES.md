# Developing KSJ Libraries

KSJ libraries use two different dependency paths depending on where the code is being used.

```text
Application
    ↓
uses released library

Library repository
    ↓
examples compile against local sources
```

## Application projects

Applications such as Prototype Box and RAMU products should depend on a released or explicitly pinned version of a KSJ library.

Example:

```ini
lib_deps =
    https://github.com/KSJain/KSJ_Storage.git#v0.2.0
```

This keeps application builds reproducible and prevents untested local library changes from silently entering a product.

## Library examples

Examples stored inside a library repository must compile against that repository's local source files.

Do not install the library into its own example through `lib_deps`.

Incorrect:

```ini
lib_deps =
    https://github.com/KSJain/KSJ_Storage.git
```

That may cause PlatformIO to download a released or cached copy of the library into `.pio/libdeps`, hiding the local files currently under development.

Correct:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino

monitor_speed = 115200
```

The example should live inside the library repository:

```text
KSJ_Storage/
├── include/
├── src/
└── examples/
    └── Basic_Session_Logger/
        ├── platformio.ini
        └── src/
            └── main.cpp
```

## After changing dependencies

Remove PlatformIO's cached build files before rebuilding:

```bash
cd examples/Basic_Session_Logger
rm -rf .pio
pio run
```

## Rule

> Never install a library into its own examples.

Library examples validate the code currently being developed.

Applications validate released library versions.
