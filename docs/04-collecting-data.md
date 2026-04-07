# 4 — Collecting Training Data

---

## Prerequisites

Before opening the browser:

- Arduino is uploaded and plugged in via USB
- Processing TM_Connector is running and showing a live camera image

---

## Open Teachable Machine

1. Open **Google Chrome** (recommended — other browsers may not support the Device input)
2. Go to https://teachablemachine.withgoogle.com/train
3. Click **Image Project**
4. Click **Embedded image model**

> You must choose "Embedded image model" (not the standard image model). Only the embedded model supports streaming from a device via WebSocket.

---

## Connect the camera

1. In the class editor, click the input type dropdown for any class and select **Device**
2. Click **"Attempt to connect to device"**
3. If the Processing sketch is running, the connection will succeed automatically and a live preview of your camera will appear inside Teachable Machine
4. In the Processing window the status dot will turn **green** and show **"Connected to TM"**

---

## Name your classes

Rename `Class 1` and `Class 2` to describe what you want to distinguish.  
Add more classes with the **"Add a class"** button if needed.

Examples: `empty` / `hand`, `open` / `closed`, `object_a` / `object_b` / `background`

---

## Record samples

1. Point the camera at the subject for the first class
2. Hold down **Record** — samples are captured at video speed
3. Aim for **at least 50 samples per class** (100+ gives better results)
4. Repeat for each class

**Tips for better models:**

- Keep lighting consistent between classes but vary it slightly within each class to improve robustness
- Vary the angle and distance of your subject within each class  
- Include a "background" or "nothing" class with the scene empty so the model has a clear negative case
- The camera sees in grayscale — colour differences between classes won't help

---

## Train

Click **Train Model**.  
Do **not** switch browser tabs while training is in progress — it will pause.

Training runs entirely in the browser and typically takes one to two minutes depending on the number of samples and classes.

---

## Test

When training finishes, Teachable Machine automatically runs the live camera feed through the model in the preview panel. Test how well it distinguishes your classes by pointing the camera at each one.

If results are poor:

- Add more samples, especially in cases that are being misclassified
- Make sure there is enough visual difference between classes at 96×96 grayscale resolution
- Retrain

---

## Export

When you're happy with the model:

1. Click **Export Model** (top-right corner of the Teachable Machine page)
2. Select **Tensorflow Lite**
3. Select **Tensorflow Lite for Microcontrollers**
4. Click **Download my model**

This downloads a ZIP file containing a complete Arduino sketch with your model compiled in. Open the `.ino` inside the ZIP in the Arduino IDE and upload it to your Mega 2560 using the same `ImageProvider.cpp` from this repo to capture images at inference time.
