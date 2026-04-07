# 1 — Library Setup

Complete this before opening either sketch.

---

## Arduino IDE

Download and install the [Arduino IDE](https://www.arduino.cc/en/software) (version 1.8.x or 2.x).

---

## ArduCAM library

1. Go to https://github.com/ArduCAM/Arduino and click **Code → Download ZIP**.
2. In the Arduino IDE: **Sketch → Include Library → Add .ZIP Library…** and select the downloaded ZIP.  
   *(Alternatively, unzip it and copy only the `ArduCAM` subfolder into your `Documents/Arduino/libraries/` folder.)*

3. **Critical — edit `memorysaver.h`.**

   Open `Documents/Arduino/libraries/ArduCAM/memorysaver.h` in any text editor and make exactly one `#define` active in the camera-module block:

   ```c
   //#define OV2640_MINI_2MP
   #define OV2640_MINI_2MP_PLUS     // ← this one only
   //#define OV2640_MINI_2MP_SHIELD
   //#define OV5642_MINI_5MP
   // ... all others commented out
   ```

   If more than one `#define` is active the sketch will not compile.

---

## Processing IDE

Download and install the [Processing IDE](https://processing.org/download/) (version 3 or 4).

---

## Processing libraries

Open Processing. In the menu bar: **Sketch → Add Library → Manage Libraries…**

Search for and install both of these:

| Library | What it does |
|---------|-------------|
| **ControlP5** | Provides the COM port dropdown in the TM_Connector window |
| **Websockets** | Runs the local WebSocket server that Teachable Machine connects to |

> If "Websockets" doesn't appear in the library manager, install it manually:  
> 1. Download from https://github.com/alexanderstreet/websockets (Code → Download ZIP)  
> 2. Unzip and place the `websockets` folder in your Processing `libraries` folder  
>    (`Documents/Processing/libraries/` on Windows/macOS)

---

Once both IDEs and all libraries are installed, continue to [02 — Arduino Setup](02-arduino-setup.md).
