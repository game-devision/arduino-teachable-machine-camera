# 3 — Processing Setup

---

## Open the sketch

Open `processing/TM_Connector/TM_Connector.pde` in the Processing IDE by double-clicking the `.pde` file.

---

## Run it

Click the **Run** button (▶ triangle, top-left of the Processing IDE) or press **Ctrl+R** (Windows/Linux) / **⌘+R** (macOS).

A window will appear:

```
┌─────────────────────────────────────────────┐
│  [camera preview area]    │  [COM port list] │
│                           │                  │
│                           │  Not Connected ● │
└─────────────────────────────────────────────┘
```

---

## Select the COM port

Click the port dropdown on the right side of the window and select the same COM port the Arduino is on (the same one you used when uploading in the Arduino IDE).

Within one or two seconds the camera image should appear in the left half of the window.

The status indicator shows **"Not Connected to TM"** with a red dot — that is expected until Teachable Machine connects. The image feed being visible confirms the Arduino → Processing link is working.

---

## Leave it running

Do **not** close the Processing window. It must stay open while you are collecting training data in the browser, because it is running the WebSocket server (`ws://localhost:8889`) that Teachable Machine connects to.

---

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| No image after selecting port | Close the Arduino Serial Monitor if it is still open; then re-select the port |
| Black or grey screen | Camera is auto-exposing — point it at something with visible light and wait a second |
| Processing crashes immediately | Check that both `ControlP5` and `Websockets` libraries are installed (see [01 — Library Setup](01-library-setup.md)) |
| Port not listed in dropdown | Arduino not detected; try a different USB cable or port on your computer |

---

Continue to [04 — Collecting Data](04-collecting-data.md).
