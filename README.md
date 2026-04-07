# arduino-teachable-machine-camera

Stream live grayscale video from an **ArduCAM Mini 2MP Plus** connected to an **Arduino Mega 2560** directly into [Google Teachable Machine](https://teachablemachine.withgoogle.com/) to collect training data for an embedded image classifier — no webcam required.

This repo adapts the official [Teachable Machine community guide](https://github.com/googlecreativelab/teachablemachine-community) (originally written for the Arduino Nano 33 BLE + OV7670) to work with the ArduCAM Mini 2MP Plus on the Arduino Mega 2560 — a more widely available hardware combination that requires no JPEG decoding library.

---

## How it works

```
ArduCAM Mini 2MP Plus (captures 320×240 RGB565)
         │ SPI
Arduino Mega 2560  ──  nearest-neighbour downsample → 96×96 grayscale
         │ USB Serial @ 500,000 baud
Processing (TM_Connector)  ──  frame-sync state machine → base64 encode
         │ WebSocket  ws://localhost:8889
Chrome browser  ──  teachablemachine.withgoogle.com  (Embedded image model, Device input)
```

---

## Hardware required

| Part | Notes |
|------|-------|
| Arduino Mega 2560 | Other AVR boards with ≥8 KB SRAM may work with minor changes |
| ArduCAM Mini 2MP Plus (OV2640) | Must be the "Plus" variant — it has the on-board FIFO chip |
| USB-B cable | For power and serial communication to the PC |

See the [ArduCAM wiring guide](https://github.com/ArduCAM/Arduino) for the full SPI + I²C pinout.  
**CS (chip-select) must be wired to digital pin 49** on the Mega 2560 — this is defined in `ImageProvider.cpp`.

---

## Quick start

| Step | Guide |
|------|-------|
| 1 | [Library setup](docs/01-library-setup.md) — ArduCAM, Processing, websockets, controlP5 |
| 2 | [Upload the Arduino sketch](docs/02-arduino-setup.md) |
| 3 | [Run the Processing sketch](docs/03-processing-setup.md) |
| 4 | [Collect training data in Teachable Machine](docs/04-collecting-data.md) |

---

## Repository structure

```
arduino/
  TM_Uploader/
    TM_Uploader.ino      — main loop; calls StreamImageToSerial() each iteration
    ImageProvider.cpp    — camera init, FIFO burst read, downsample, serial output
    ImageProvider.h      — function declarations

processing/
  TM_Connector/
    TM_Connector.pde     — serial receiver, frame-sync state machine, WebSocket server

docs/
  01-library-setup.md
  02-arduino-setup.md
  03-processing-setup.md
  04-collecting-data.md
```

---

## Technical notes

- **No JPEG decoder needed.** The OV2640 is put into BMP (raw RGB565) mode. Pixels are converted to grayscale on the fly during the SPI burst read, so no frame buffer is needed and the Mega's 8 KB SRAM is not a constraint.
- **Full field of view.** The 320×240 frame is nearest-neighbour downsampled to 96×96 rather than cropped, so the camera sees the same scene width as you do.
- **Reliable framing.** Each frame on the serial line is prefixed with a `0xFF 0xAA` sync header. Pixel values are capped at `0xFE` so `0xFF` is reserved and can never cause a false sync detection.

---

## Troubleshooting

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| Static or no image | CS pin wrong or wiring loose | Confirm CS wire is on pin 49; check all SPI connections |
| All-grey image | Camera not exposing | Point camera at a brighter scene; OV2640 auto-exposes |
| "SPI interface Error!" in Serial Monitor | Bad SPI connection | Re-check MOSI / MISO / SCK / CS wiring |
| Port not in Processing dropdown | Arduino IDE Serial Monitor still open | Close Serial Monitor before running Processing |
| Red "Not Connected to TM" dot | Processing sketch not running / wrong WebSocket port | Ensure Processing sketch is running *before* clicking "Attempt to connect" in TM |
| Image appears mirrored | OV2640 mirror register | Swap `lo` and `hi` in the SPI read block in `ImageProvider.cpp` |

---

## License

Apache 2.0 — see [LICENSE](LICENSE).  
Portions copyright 2019–2021 The TensorFlow Authors / Google LLC.
