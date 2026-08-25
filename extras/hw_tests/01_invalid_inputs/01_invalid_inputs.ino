/*
 * GP8403 Invalid Input Hardware Test
 *
 * Verifies that invalid raw values, voltages, and channels are rejected.
 */

#include <Adafruit_GP8403.h>
#include <math.h>

Adafruit_GP8403 gp8403;

void setup() {
  Serial.begin(115200);
  Serial.println(F("GP8403 invalid input hardware test"));

  if (gp8403.saveToNVM(SDA, SCL)) {
    halt(F("INVALID_INPUTS FAIL: save before begin"));
  }
  Serial.println(F("Save before begin correctly rejected"));

  if (!gp8403.begin()) {
    halt(F("INVALID_INPUTS FAIL: begin"));
  }
  Serial.println(F("Begin succeeded"));

  if (gp8403.setRaw(0, 4096)) {
    clearOutputsAndHalt(F("INVALID_INPUTS FAIL: raw value"));
  }
  Serial.println(F("Invalid raw value correctly rejected"));

  if (gp8403.setVoltage(0, -0.1)) {
    clearOutputsAndHalt(F("INVALID_INPUTS FAIL: negative voltage"));
  }
  Serial.println(F("Negative voltage correctly rejected"));

  if (gp8403.setVoltage(1, 5.1)) {
    clearOutputsAndHalt(F("INVALID_INPUTS FAIL: voltage above range"));
  }
  Serial.println(F("Voltage above range correctly rejected"));

  if (gp8403.setVoltage(0, NAN)) {
    clearOutputsAndHalt(F("INVALID_INPUTS FAIL: NAN voltage"));
  }
  Serial.println(F("NAN voltage correctly rejected"));

  if (gp8403.setVoltage(1, INFINITY)) {
    clearOutputsAndHalt(F("INVALID_INPUTS FAIL: infinite voltage"));
  }
  Serial.println(F("Infinite voltage correctly rejected"));

  if (gp8403.setVoltage(2, 1.0)) {
    clearOutputsAndHalt(F("INVALID_INPUTS FAIL: channel"));
  }
  Serial.println(F("Invalid channel correctly rejected"));

  if (!gp8403.setVoltages(0.0, 0.0)) {
    clearOutputsAndHalt(F("INVALID_INPUTS FAIL: set zero volts"));
  }
  Serial.println(F("Set zero volts succeeded"));

  clearOutputsAndHalt(F("INVALID_INPUTS PASS"));
}

void loop() {}

void clearOutputsAndHalt(const __FlashStringHelper *message) {
  Serial.println(message);
  if (!gp8403.setVoltages(0.0, 0.0)) {
    Serial.println(F("INVALID_INPUTS FAIL: reset outputs"));
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
