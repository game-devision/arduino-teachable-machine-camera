/* Copyright 2021 Google LLC All Rights Reserved.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  ==============================================================================
 */

import processing.serial.*;
import websockets.*;
import javax.xml.bind.DatatypeConverter;
import controlP5.*;
import java.util.*;

Serial myPort;
WebsocketServer ws;

// Must match the resolution configured in TM_Uploader.ino
final int cameraWidth = 96;
final int cameraHeight = 96;
final int cameraBytesPerPixel = 1;
final int bytesPerFrame = cameraWidth * cameraHeight * cameraBytesPerPixel;

PImage myImage;
byte[] frameBuffer = new byte[bytesPerFrame];
String[] portNames;
ControlP5 cp5;
ScrollableList portsList;
boolean clientConnected = false;

void setup() {
  size(448, 224);
  pixelDensity(displayDensity());
  frameRate(30);
  cp5 = new ControlP5(this);
  portNames = Serial.list();
  portNames = filteredPorts(portNames);
  ws = new WebsocketServer(this, 8889, "/");
  portsList = cp5.addScrollableList("portSelect")
    .setPosition(235, 10)
    .setSize(200, 220)
    .setBarHeight(40)
    .setItemHeight(40)
    .addItems(portNames);

  portsList.close();
  myImage = createImage(cameraWidth, cameraHeight, RGB);
  noStroke();
}

void draw() {
  background(240);
  image(myImage, 0, 0, 224, 224);
  drawConnectionStatus();
}

void drawConnectionStatus() {
  fill(0);
  textAlign(RIGHT, CENTER);
  if (!clientConnected) {
    text("Not Connected to TM", 410, 100);
    fill(255, 0, 0);
  } else {
    text("Connected to TM", 410, 100);
    fill(0, 255, 0);
  }
  ellipse(430, 102, 10, 10);
}

void portSelect(int n) {
  String selectedPortName = (String) cp5.get(ScrollableList.class, "portSelect").getItem(n).get("text");
  try {
    myPort = new Serial(this, selectedPortName, 500000);
  }
  catch (Exception e) {
    println(e);
  }
}

boolean stringFilter(String s) {
  return (!s.startsWith("/dev/tty"));
}

String[] filteredPorts(String[] ports) {
  int n = 0;
  for (String portName : ports) if (stringFilter(portName)) n++;
  String[] retArray = new String[n];
  n = 0;
  for (String portName : ports) if (stringFilter(portName)) retArray[n++] = portName;
  return retArray;
}

// ─── Frame reception ──────────────────────────────────────────────────────────
//
// The Arduino prefixes every frame with the two-byte sync header 0xFF 0xAA.
// Pixel values are capped at 0xFE on the Arduino side, so 0xFF never appears
// in legitimate pixel data — false sync detection is impossible.

int lastFrame = -1;
// 0xFF is reserved on the Arduino side (pixel values capped at 0xFE), so
// 0xFF 0xAA can never appear in legitimate pixel data — zero false-sync risk.
int syncState = 0;  // 0 = looking for 0xFF, 1 = looking for 0xAA, 2 = reading frame
int frameIndex = 0;

void serialEvent(Serial p) {
  byte[] incoming = p.readBytes();
  if (incoming == null) return;

  for (int idx = 0; idx < incoming.length; idx++) {
    int b = incoming[idx] & 0xFF;

    switch (syncState) {
      case 0:  // waiting for 0xFF
        if (b == 0xFF) syncState = 1;
        break;
      case 1:  // waiting for 0xAA (consecutive 0xFF bytes are fine: stay in state 1)
        if (b == 0xAA) {
          syncState = 2;
          frameIndex = 0;
        } else if (b != 0xFF) {
          syncState = 0;
        }
        break;
      case 2:  // accumulating frame bytes
        if (b == 0xFF) {
          // 0xFF is reserved — treat as start of a new sync, discarding partial frame
          syncState = 1;
        } else {
          frameBuffer[frameIndex++] = (byte) b;
          if (frameIndex >= bytesPerFrame) {
            processFrame();
            syncState = 0;
          }
        }
        break;
    }
  }
}

void processFrame() {
  for (int i = 0; i < bytesPerFrame; i++) {
    int r = (int)(frameBuffer[i] & 0xFF);
    myImage.pixels[i] = color(r, r, r);
  }
  myImage.updatePixels();

  if (lastFrame == -1) {
    lastFrame = millis();
  } else {
    int frameTime = millis() - lastFrame;
    print("fps: ");
    println(frameTime);
    lastFrame = millis();
  }

  String data = DatatypeConverter.printBase64Binary(frameBuffer);
  ws.sendMessage(data);
}

void webSocketServerEvent(String msg) {
  if (msg.equals("tm-connected")) clientConnected = true;
}
