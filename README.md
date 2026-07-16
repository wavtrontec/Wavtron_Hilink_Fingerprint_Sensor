# HLK-ZW0922 Fingerprint Sensor — ESP32

Enroll and match fingerprints on the **HLK-ZW0922** capacitive fingerprint module using an ESP32, over UART. Includes a beginner-friendly demo sketch with clean serial output — ready to film, ready to build on.

<img width="1500" height="1500" alt="image" src="https://github.com/user-attachments/assets/17b1ea0a-8a4d-4969-aecf-f2d8c62b498e" />


## Features

- Enroll new fingerprints (2-scan capture + merge)
- Real-time match / identify mode with confidence score
- LED feedback on the sensor (green = match, red = no match)
- Timeout + heartbeat logging — no silent hangs
- Built on the EF-01 UART protocol (same family as R30x/AS608 sensors)

## Hardware

| Sensor Pin | ESP32 Pin |
|---|---|
| SENSOR_3.3V | 3V3 |
| MCU_3.3V | 3V3 |
| GND | GND |
| TX | GPIO16 (RX2) |
| RX | GPIO17 (TX2) |
| WAKEUP | not connected (optional, for interrupt wake) |

- Sensor: capacitive, 508 DPI, UART TTL 3.3V, 57600 baud default
- Tested on: ESP32-S3 DevKitC (Type-C)
- **Do not power at 5V** — max supply is 3.6V

## Getting Started

1. Wire the sensor per the table above.
2. Install the `HLK_fingerprint` library (Arduino IDE → Sketch → Include Library → Add .ZIP, or copy into `libraries/`).
3. Flash `hlk_fp_esp32_beginner.ino`.
4. Open Serial Monitor @ 115200 baud.
5. Type `1` + Enter → enroll a fingerprint. Type `2` + Enter → start scanning.

## How it Works

The sensor runs its own onboard MCU that handles image capture, feature extraction, template storage, and 1:N matching entirely on-chip. The ESP32 only sends short UART commands (`GetImage`, `Image2Tz`, `RegModel`, `Store`, `HiSpeedSearch`, etc.) and reads back a confirm code — it never touches raw fingerprint image data.

## License

MIT — see [LICENSE](LICENSE)

## Credits

Built by [Wavtron](https://wavtron.in) — electronics brand for makers and SMEs
