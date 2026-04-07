# 2 — Arduino Setup

---

## Open the sketch

Open `arduino/TM_Uploader/TM_Uploader.ino` in the Arduino IDE by double-clicking the `.ino` file.  
The IDE will automatically load `ImageProvider.cpp` and `ImageProvider.h` from the same folder — do not move them.

---

## Select the board

**Tools → Board → Arduino AVR Boards → Arduino Mega or Mega 2560**  
Then: **Tools → Processor → ATmega2560 (Mega 2560)**

---

## Select the port

Plug in your Arduino Mega 2560 via USB, then:  
**Tools → Port** → select the port for your board.

| OS | Port format |
|----|-------------|
| Windows | `COM3`, `COM4`, … |
| macOS | `/dev/cu.usbmodem…` |
| Linux | `/dev/ttyACM0`, `/dev/ttyUSB0` |

If multiple ports appear and you're unsure which one, unplug the Arduino, note which ports are listed, plug it back in, and the new entry is your board.

---

## Upload

Click the **Upload** button (→ arrow icon) or press **Ctrl+U** (Windows/Linux) / **⌘+U** (macOS).  
Compilation takes about 30 seconds the first time.

---

## Verify it's working

Open **Tools → Serial Monitor** and set the baud rate dropdown (bottom-right) to **500000**.

Expected output:

```
ACK CMD ArduCAM Start! END
ACK CMD SPI interface OK. END
ACK CMD OV2640 detected. END
```

If you see `SPI interface Error!` or the OV2640 is not detected:

- Re-check all SPI wires (MOSI, MISO, SCK, CS)
- Confirm the CS wire goes to **digital pin 49** on the Mega
- Confirm I²C wires (SDA → pin 20, SCL → pin 21 on the Mega)
- Make sure `OV2640_MINI_2MP_PLUS` is the only active define in `memorysaver.h`

> **Before opening Processing:** close the Serial Monitor. Only one program can hold the COM port open at a time.

---

Continue to [03 — Processing Setup](03-processing-setup.md).
