# ESP32 Power Consumption Benchmark

A collection of power-consumption measurements for ESP32 development boards.

The project uses one Arduino sketch to run comparable workloads on different ESP32 families:

- ESP32
- ESP32-S2
- ESP32-S3
- ESP32-C3
- ESP32-C5
- Other compatible ESP32 variants

The goal is to compare the power consumption of complete development boards, including their voltage regulators, USB interfaces, LEDs and other onboard components.

## Hardware

### Initial board: ESP32-C3 dual USB-C

The first test board has:

- ESP32-C3 processor
- Two USB-C connectors labelled `USB` and `COM`
- 15 pins on each side
- Onboard addressable RGB LED

The exact board manufacturer and model have not yet been identified.

| Component | Configuration |
|---|---|
| Processor | ESP32-C3 |
| CPU frequency | 160 MHz |
| RGB LED | Addressable RGB |
| RGB GPIO | 8 (provisional) |
| Serial interface | COM / USB-to-UART |
| USB interface | Native USB (presumed) |

GPIO8 is a common RGB LED connection on ESP32-C3 development boards. Verify this against the schematic or by testing before using it on other boards.

## Software

- Arduino IDE
- Espressif Arduino-ESP32 core
- Adafruit NeoPixel library

### Installation

1. Install the ESP32 Arduino platform.
2. Install the Adafruit NeoPixel library.
3. Open `powerconsumption.ino`.
4. Select an appropriate ESP32-C3 board.
5. Connect using the COM USB-C connector.
6. Open Serial Monitor at 115200 baud.

For boards using native USB CDC, the appropriate USB CDC setting may be required.

## Tests

The sketch provides the following tests:

| Command | Test | Description |
|---|---|---|
| `i` | Idle | CPU running, Wi-Fi disabled |
| `c` | CPU active | Continuous CPU workload |
| `w` | Wi-Fi enabled | Wi-Fi station enabled, disconnected |
| `s` | Wi-Fi scan | Scan available 2.4 GHz networks |
| `l` | Light sleep | Timer-based light sleep |
| `d` | Deep sleep | Timer-based deep sleep |
| `r` | RGB LED | Red, green, blue and white |
| `a` | All tests | Run complete sequence |

The default test durations are 30 seconds, except for the RGB LED test and the Wi-Fi scan.

Deep sleep restarts the processor after the timer expires. The wake-up cause is reported after reboot.

## Measurement procedure

### Power measurement

Use an external power meter or ammeter between the USB power source and the development board.

Recommended equipment:

- USB power meter for general consumption measurements.
- Bench power supply with current measurement.
- INA219 or INA226 for automated measurements.
- A suitable low-current ammeter for deep-sleep measurements.

Measure the complete development board, not just the ESP32 processor.

### Important considerations

- Use the same power supply voltage for all boards.
- Use the same measurement equipment and wiring.
- Disconnect external sensors and peripherals.
- Keep the RGB LED switched off except during its dedicated test.
- Allow the board to stabilize before recording current.
- Record whether the board is powered through USB, COM or an external supply.
- Do not connect two independently powered USB connectors simultaneously unless the board documentation explicitly permits it.
- USB-to-UART converters and voltage regulators can contribute significantly to the measured current.

USB power meters may not have sufficient resolution for microamp-level deep-sleep measurements.

### Measurement sequence

For each board:

1. Flash the same benchmark sketch, with the appropriate board configuration.
2. Connect the power meter.
3. Start the Serial Monitor.
4. Run each test individually.
5. Record average current and, where relevant, peak current.
6. Repeat each test at least three times.
7. Record the measurement conditions.

For deep sleep, wait until the board has entered sleep and the current has stabilized before recording.

## Measurement results

All current values are in mA unless otherwise specified.

### Board overview

| Board | Processor | Flash | RGB LED | Status |
|---|---|---:|---|---|
| ESP32-C3 dual USB-C | ESP32-C3 | TBD | Yes | Initial |
| WEMOS S2 Mini | ESP32-S2 | TBD | TBD | Planned |
| ESP32-C3 Super Mini | ESP32-C3 | TBD | Yes, variant dependent | Planned |
| LOLIN S3 | ESP32-S3 | TBD | Yes, variant dependent | Planned |

### Current consumption comparison

| Test | C3 dual USB-C | S2 Mini | C3 Super Mini | LOLIN S3 |
|---|---:|---:|---:|---:|
| Idle | — | — | — | — |
| CPU active | — | — | — | — |
| Wi-Fi enabled | — | — | — | — |
| Wi-Fi scan | — | — | — | — |
| Light sleep | — | — | — | — |
| Deep sleep | — | — | — | — |
| RGB red | — | — | — | — |
| RGB green | — | — | — | — |
| RGB blue | — | — | — | — |
| RGB white | — | — | — | — |

### Detailed measurement log

| Board | Test | Average (mA) | Peak (mA) | Voltage (V) | Notes |
|---|---|---:|---:|---:|---|
| ESP32-C3 | Idle | — | — | — | |
| ESP32-C3 | CPU active | — | — | — | |
| ESP32-C3 | Wi-Fi enabled | — | — | — | |
| ESP32-C3 | Wi-Fi scan | — | — | — | |
| ESP32-C3 | Light sleep | — | — | — | |
| ESP32-C3 | Deep sleep | — | — | — | |

## Future improvements

- [ ] Add automatic board identification.
- [ ] Add board-specific configuration profiles.
- [ ] Add RGB LED identification and configuration.
- [ ] Add BLE scanning.
- [ ] Add Wi-Fi scanning at configurable intervals.
- [ ] Add Wi-Fi connection and data transmission tests.
- [ ] Add configurable CPU frequencies.
- [ ] Add automated INA219/INA226 measurements.
- [ ] Export results as CSV and JSON.
- [ ] Add automated benchmark comparison.
- [ ] Add battery-life estimates based on measured consumption.

## Repository

[github.com/kreier/esp32-powerconsumption](https://github.com/kreier/esp32-powerconsumption)

