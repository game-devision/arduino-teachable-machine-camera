/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "ImageProvider.h"

/*
 * Arduino Mega 2560 + ArduCAM Mini 2MP Plus (OV2640)
 *
 * The OV2640 is put into raw RGB565 (BMP) mode. It always captures at
 * QVGA (320x240) in this mode regardless of any JPEG size setting.
 * The full 320x240 frame is nearest-neighbour downsampled to 96x96
 * grayscale on the fly during the SPI burst read — no frame buffer needed.
 *
 * Each frame on the serial line is prefixed with the two-byte sync header
 * 0xFF 0xAA. Pixel values are capped at 0xFE so that 0xFF is reserved and
 * can never trigger a false sync in the Processing receiver.
 *
 * SRAM footprint: 560 bytes static (downsample maps) + ~30 bytes stack.
 *
 * Requires:
 *   ArduCAM library  https://github.com/ArduCAM/Arduino
 *   memorysaver.h    uncomment #define OV2640_MINI_2MP_PLUS only
 */

#include <SPI.h>
#include <Wire.h>
#include <memorysaver.h>
#include <ArduCAM.h>

#if !(defined OV2640_MINI_2MP_PLUS)
#error Please enable OV2640_MINI_2MP_PLUS in ArduCAM/memorysaver.h
#endif

// Chip-select pin — wire ArduCAM CS to digital pin 49 on the Mega 2560
#define CS 49

// OV2640 BMP mode always outputs QVGA regardless of JPEG size register
#define CAP_W 320
#define CAP_H 240

static ArduCAM myCAM(OV2640, CS);

static bool InitCamera() {
  Wire.begin();
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  SPI.begin();
  // ArduCAM 2MP Plus CPLD is rated max 8 MHz — do not exceed
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  // Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);

  // Verify SPI bus
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  if (myCAM.read_reg(ARDUCHIP_TEST1) != 0x55) {
    return false;
  }

  // Raw RGB565 mode
  myCAM.set_format(BMP);
  myCAM.InitCAM();
  delay(100);
  return true;
}

bool StreamImageToSerial(int image_width, int image_height, int channels) {
  static bool g_is_camera_initialized = false;
  if (!g_is_camera_initialized) {
    if (!InitCamera()) return false;
    g_is_camera_initialized = true;
  }

  // Trigger capture into FIFO
  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {}
  delay(50);  // FIFO needs ~50 ms to stabilise before burst read

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  SPI.transfer(0x00);  // dummy byte required after burst-mode enable

  // Sync header — 0xFF is reserved so this can never appear in pixel data
  Serial.write((uint8_t)0xFF);
  Serial.write((uint8_t)0xAA);

  // Build nearest-neighbour downsample maps once (320 bytes + 240 bytes static)
  static bool s_row_sampled[CAP_H];
  static bool s_col_sampled[CAP_W];
  static bool s_maps_built = false;
  if (!s_maps_built) {
    memset(s_row_sampled, 0, sizeof(s_row_sampled));
    memset(s_col_sampled, 0, sizeof(s_col_sampled));
    for (int oy = 0; oy < image_height; oy++)
      s_row_sampled[(oy * CAP_H) / image_height] = true;
    for (int ox = 0; ox < image_width; ox++)
      s_col_sampled[(ox * CAP_W) / image_width] = true;
    s_maps_built = true;
  }

  // Drain the full 320x240 FIFO; output only the sampled pixels
  for (int y = 0; y < CAP_H; y++) {
    for (int x = 0; x < CAP_W; x++) {
      // RGB565 little-endian: low byte = G[2:0] B[4:0], high byte = R[4:0] G[5:3]
      uint8_t lo = SPI.transfer(0x00);
      uint8_t hi = SPI.transfer(0x00);

      if (s_row_sampled[y] && s_col_sampled[x]) {
        uint16_t color = ((uint16_t)hi << 8) | lo;
        uint8_t r = ((color >> 11) & 0x1F) << 3;  // 5-bit → 8-bit
        uint8_t g = ((color >>  5) & 0x3F) << 2;  // 6-bit → 8-bit
        uint8_t b = ( color        & 0x1F) << 3;  // 5-bit → 8-bit
        // BT.601 luminance (integer, coefficients sum to 255)
        uint8_t gray = (uint8_t)(((uint16_t)54 * r +
                                  (uint16_t)183 * g +
                                  (uint16_t)18  * b) >> 8);
        if (gray == 0xFF) gray = 0xFE;  // reserve 0xFF for sync header
        Serial.write(gray);
      }
    }
  }

  myCAM.CS_HIGH();
  return true;
}

// Kept to satisfy ImageProvider.h — not called by TM_Uploader
bool GetImage(int image_width, int image_height, int channels, uint8_t* image_data) {
  return StreamImageToSerial(image_width, image_height, channels);
}

bool GetDummyImage(int image_width, int image_height, int channels, uint8_t* image_data) {
  for (int y = 0; y < image_height; y++)
    for (int x = 0; x < image_width; x++)
      image_data[y * image_width + x] = (uint8_t)(x + y);
  return true;
}
