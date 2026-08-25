/*
 * GP8403 Basic Hardware Test
 *
 * Verifies repeatable initialization and two output voltages in the 5 V range.
 */

#include <Adafruit_GP8403.h>

Adafruit_GP8403 gp8403;

void setup() {
  Serial.begin(115200);
  Serial.println(F("GP8403 basic hardware test"));

  if (!gp8403.begin()) {
    halt(F("BASIC FAIL: first begin"));
  }
  Serial.println(F("First begin succeeded"));

  if (!gp8403.begin()) {
    clearOutputsAndHalt(F("BASIC FAIL: second begin"));
  }
  Serial.println(F("Second begin succeeded"));

  if (!gp8403.setVoltages(1.0, 3.0)) {
    clearOutputsAndHalt(F("BASIC FAIL: set voltages"));
  }
  Serial.println(F("Set voltages succeeded"));

  delay(10);

  // The 2:1 dividers map 0-1023 ADC counts to 0-10 V at the outputs.
  float a0Volts = map(readADC(A0, 32), 0, 1023, 0, 10000) / 1000.0;
  float a1Volts = map(readADC(A1, 32), 0, 1023, 0, 10000) / 1000.0;
  Serial.print(F("A0="));
  Serial.print(a0Volts, 3);
  Serial.print(F(" V A1="));
  Serial.print(a1Volts, 3);
  Serial.println(F(" V"));

  if (!gp8403.setVoltages(0.0, 0.0)) {
    clearOutputsAndHalt(F("BASIC FAIL: set zero volts"));
  }
  Serial.println(F("Set zero volts succeeded"));

  if (abs(a0Volts - 1.0) > 0.25 || abs(a1Volts - 3.0) > 0.25) {
    clearOutputsAndHalt(F("BASIC FAIL: ADC readings"));
  }
  Serial.println(F("ADC readings succeeded"));

  clearOutputsAndHalt(F("BASIC PASS"));
}

void loop() {}

uint16_t readADC(uint16_t pin, uint8_t samples) {
  uint32_t total = 0;
  for (uint8_t i = 0; i < samples; i++) {
    total += analogRead(pin);
  }
  return total / samples;
}

void clearOutputsAndHalt(const __FlashStringHelper *message) {
  Serial.println(message);
  if (!gp8403.setVoltages(0.0, 0.0)) {
    Serial.println(F("BASIC FAIL: reset outputs"));
  }

  while (true) {
    delay(1000);
  }
}

void halt(const __FlashStringHelper *message) {
  Serial.println(message);
  while (true) {
    delay(1000);
  }
}
