/*
 * ============================================================================
 *  Project     : HLK-ZW0922 Fingerprint Sensor + ESP32 — Beginner Demo
 *  Description : Enroll and match fingerprints on the HLK-ZW0922 capacitive
 *                fingerprint module over UART, with clean serial output.
 * ----------------------------------------------------------------------------
 *  Author      : Aamir | Wavtron (https://wavtron.in)
 *  Created     : 2026-07-16
 *  Version     : 1.0.0
 *  Board       : ESP32-S3 DevKitC (Type-C)
 *  Library     : HLK_fingerprint v1.1.1
 * ----------------------------------------------------------------------------
 *  Usage       : Serial Monitor @ 115200 baud
 *                Type 1 + Enter  -> Enroll a new fingerprint
 *                Type 2 + Enter  -> Start scanning / matching mode
 * ----------------------------------------------------------------------------
 *  Wiring:
 *    Sensor TX               -> ESP32 GPIO16 (RX2)
 *    Sensor RX               -> ESP32 GPIO17 (TX2)
 *    SENSOR_3.3V + MCU_3.3V  -> ESP32 3V3
 *    GND                     -> ESP32 GND
 * ----------------------------------------------------------------------------
 *  License     : MIT — see LICENSE file in repo root
 *  Copyright   : (c) 2026 Wavtron. Permission is granted, free of charge, to
 *                any person obtaining a copy of this software and associated
 *                documentation files, to deal in the software without
 *                restriction, subject to inclusion of this copyright notice.
 * ============================================================================
 */

#include <HLK_fingerprint.h>

#define FP_RX 16
#define FP_TX 17

FingerprintModule fp(Serial1, FP_RX, FP_TX);

// ---------- helpers ----------

uint16_t readNumber() {
  while (!Serial.available()) delay(1);
  uint16_t v = (uint16_t)Serial.parseInt();
  while (Serial.available()) Serial.read();
  return v;
}

// Waits for a finger to be placed. Returns true once captured.
bool waitForFinger(uint32_t timeoutMs) {
  uint32_t start = millis();
  while (!fp.getImage()) {
    if (fp.lastCC != 0x02) return false;          // real error
    if (millis() - start > timeoutMs) return false; // gave up
  }
  return true;
}

// Waits until the finger is lifted off the sensor.
void waitForLift(uint32_t timeoutMs) {
  uint32_t start = millis();
  do {
    delay(100);
    fp.getImage();
    if (millis() - start > timeoutMs) return;
  } while (fp.lastCC != 0x02);
}

// ---------- enroll ----------

void enrollFinger() {
  Serial.println();
  Serial.print(F("Choose a slot number (0-"));
  Serial.print(fp.capacity() - 1);
  Serial.println(F("):"));
  uint16_t id = readNumber();
  if (id >= fp.capacity()) {
    Serial.println(F("That number is too high. Try again."));
    return;
  }

  Serial.println(F(">> Place your finger on the sensor..."));
  if (!waitForFinger(15000)) {
    Serial.println(F("Didn't feel a finger. Let's try again — send 1."));
    return;
  }
  Serial.println(F("Got it! Now lift your finger off."));
  fp.image2Tz(1);

  waitForLift(10000);

  Serial.println(F(">> Place the SAME finger again..."));
  if (!waitForFinger(15000)) {
    Serial.println(F("Didn't feel a finger. Let's try again — send 1."));
    return;
  }
  fp.image2Tz(2);

  Serial.println(F("Saving your fingerprint..."));
  if (fp.createModel() && fp.storeTemplate(1, id)) {
    Serial.print(F("SUCCESS! Fingerprint saved as slot ")); Serial.println(id);
    fp.ledFlash(FP_LED_GREEN, 2);
  } else {
    Serial.println(F("Hmm, that didn't work — make sure you used the same finger both times."));
    fp.ledFlash(FP_LED_RED, 2);
  }
}

// ---------- match ----------

void matchFinger() {
  uint16_t score = 0;
  int16_t id = fp.matchFingerprint(score);

  if (id >= 0) {
    Serial.print(F("MATCH FOUND -> Slot ")); Serial.println(id);
    fp.ledFlash(FP_LED_GREEN, 2);
    delay(800);
    fp.ledBreathing(FP_LED_BLUE);
  } else if (id == -1) {
    Serial.println(F("Not recognized."));
    fp.ledFlash(FP_LED_RED, 3);
    delay(800);
    fp.ledBreathing(FP_LED_BLUE);
  }
  // id == -2: no finger placed yet, just keep waiting silently
}

// ---------- setup / loop ----------

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println();
  Serial.println(F("=========================================="));
  Serial.println(F("   HLK-ZW0922 Fingerprint Sensor Demo"));
  Serial.println(F("=========================================="));

  if (!fp.begin()) {
    Serial.println(F("Sensor not found. Check your wiring!"));
    while (true) delay(1000);
  }

  Serial.println(F("Sensor connected!"));
  Serial.println();
  Serial.println(F("Type 1 and press Enter -> Enroll a fingerprint"));
  Serial.println(F("Type 2 and press Enter -> Start scanning mode"));
  Serial.println();

  fp.ledBreathing(FP_LED_BLUE);
}

bool scanning = false;

void loop() {
  if (Serial.available()) {
    int choice = Serial.parseInt();
    while (Serial.available()) Serial.read();

    if (choice == 1) {
      scanning = false;
      enrollFinger();
    } else if (choice == 2) {
      scanning = true;
      Serial.println();
      Serial.println(F(">> Scanning mode ON — place a finger on the sensor"));
    }
  }

  if (scanning) matchFinger();
}
