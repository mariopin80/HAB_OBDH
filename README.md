# HAB OBDH — SAIRUS

[Leia em português](README.pt-BR.md)

Arduino flight software for the high-altitude balloon on-board computer (OBC/OBDH) developed at **IFSP, Boituva campus**, by Prof. Mario Pin. It acquires GPS, temperature, barometric and power measurements, records them to an SD card and sends binary telemetry over an Ebyte E32 radio.

## Flight versions

| Mission | Firmware |
| --- | --- |
| SAIRUS I | [v1.10](https://github.com/mariopin80/HAB_OBDH/releases/tag/v1.10) |
| SAIRUS II | [v2.7](https://github.com/mariopin80/HAB_OBDH/releases/tag/v2.7) |

Versions v3.0, v3.1 and v4.0 are experimental post-flight developments. The headers of v3.1 and v4.0 record their hardware test status as untested.

## Version history

The 23 sketches recovered from the original archive are available in [Releases](https://github.com/mariopin80/HAB_OBDH/releases). Each entry in the [changelog](CHANGELOG.md) identifies the original filename, links to the source and compares it with the preceding version. The Git history was reconstructed from the archived files. Dates in the changelog come from the sketch headers.

The version labels follow the archived filenames. The initial sketch gained a version header and the labels in v1.5, v1.7b and v3.1 were corrected. The rest of each sketch was verified byte for byte against the archive.

## Open a sketch

Download **Source code (zip)** from the release you want, extract it and open `HAB_OBDH/HAB_OBDH.ino` in the Arduino IDE. Use the Arduino board and core for the target OBDH hardware. The sketch on `main` is the v4.0 development version.

The sketches use `SoftwareSerial`, `TinyGPS`, the `EBYTE.h` E32 library, `OneWire`, `DallasTemperature`, `Adafruit_BMP085` and `SdFat`. Install `LowPower` for v1.7b, v1.8, v1.9 and v2.6 onward. v3.0 onward also uses the AVR core's `<avr/wdt.h>`. The v2.0.0 through v2.1 source names SdFat 2.1.0. For the E32, use the library that provides the archived sketch's `EBYTE` class and `SendStruct()` API.

## Connections in the flight sketches

Pin directions refer to the OBDH. Both serial links start at 9600 baud.

| Signal | v1.10 | v2.7 |
| --- | --- | --- |
| E32 serial RX / TX | D7 / D6 | D7 / D6 |
| E32 M0 / M1 | D4 / D5 | D4 / D5 |
| GPS serial RX / TX | D8 / D9 | D8 / D9 |
| OneWire temperature data | D3 | D3 |
| SD SPI chip select | D10 | D10 |
| Battery sense | A0 | A1 |
| Solar panel sense | A1 | A0 |

The v2.7 sketch selects `analogReference(EXTERNAL)` and records a 3.3 V external reference for its hardware. The analog values are stored in ADC counts. The SD log is written to `teste3.txt`. The radio sends the raw `DATA` struct, so the receiver needs the field layout of the corresponding firmware version.
