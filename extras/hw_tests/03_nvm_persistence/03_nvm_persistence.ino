/*
 * GP8403 NVM Persistence Hardware Test
 *
 * Saves 1 V / 2 V, verifies restoration, saves zero, and verifies zero after
 * a second complete power cycle.
 */

#include <Adafruit_GP8403.h>

Adafruit_GP8403 gp8403;

void setup() {
  Serial.begin(115200);
  Serial.println(F("GP8403 NVM persistence hardware test"));

  // The 2:1 dividers map 0-1023 ADC counts to 0-10 V at the outputs.
  float a0Volts = map(readADC(A0, 32), 0, 1023, 0, 10000) / 1000.0;
  float a1Volts = map(readADC(A1, 32), 0, 1023, 0, 10000) / 1000.0;
  Serial.print(F("POWER_UP A0="));
  Serial.print(a0Volts, 3);
  Serial.print(F(" V A1="));
  Serial.print(a1Volts, 3);
  Serial.println(F(" V"));

  char command = waitForCommand();

  if (command == 'S') {
    Serial.println(F("Save command received"));

    if (!gp8403.begin()) {
      halt(F("SAVE FAIL: begin"));
    }
    Serial.println(F("Begin succeeded"));

    if (!gp8403.setVoltages(1.0, 2.0)) {
      clearOutputsAndHalt(F("SAVE FAIL: set 1 V / 2 V"));
    }
    Serial.println(F("Set 1 V / 2 V succeeded"));

    delay(10);
    a0Volts = map(readADC(A0, 32), 0, 1023, 0, 10000) / 1000.0;
    a1Volts = map(readADC(A1, 32), 0, 1023, 0, 10000) / 1000.0;
    Serial.print(F("A0="));
    Serial.print(a0Volts, 3);
    Serial.print(F(" V A1="));
    Serial.print(a1Volts, 3);
    Serial.println(F(" V"));
    if (abs(a0Volts - 1.0) > 0.25 || abs(a1Volts - 2.0) > 0.25) {
      clearOutputsAndHalt(F("SAVE FAIL: 1 V / 2 V readings"));
    }
    Serial.println(F("1 V / 2 V readings succeeded"));

    if (!saveNVM()) {
      clearOutputsAndHalt(F("SAVE FAIL: NVM write"));
    }
    Serial.println(F("NVM write succeeded"));

    if (!gp8403.setVoltages(0.0, 0.0)) {
      clearOutputsAndHalt(F("SAVE FAIL: clear outputs"));
    }
    Serial.println(F("Clear outputs succeeded"));

    clearOutputsAndHalt(F("SAVE PASS - POWER CYCLE"));
  } else if (command == 'Z') {
    Serial.println(F("Restore command received"));

    if (!gp8403.begin()) {
      halt(F("RESTORE FAIL: begin"));
    }
    Serial.println(F("Begin succeeded"));

    if (abs(a0Volts - 1.0) > 0.25 || abs(a1Volts - 2.0) > 0.25) {
      clearOutputsAndHalt(F("RESTORE FAIL: 1 V / 2 V readings"));
    }
    Serial.println(F("1 V / 2 V readings succeeded"));

    if (!saveNVM()) {
      clearOutputsAndHalt(F("RESTORE FAIL: save zero"));
    }
    Serial.println(F("Save zero succeeded"));

    if (!gp8403.setVoltages(1.0, 1.0)) {
      clearOutputsAndHalt(F("RESTORE FAIL: set 1 V / 1 V"));
    }
    Serial.println(F("Set 1 V / 1 V succeeded"));

    halt(F("RESTORE PASS - ZERO SAVED - POWER CYCLE"));
  } else if (command == 'V') {
    Serial.println(F("Verify zero command received"));

    if (!gp8403.begin()) {
      halt(F("ZERO RESTORE FAIL: begin"));
    }
    Serial.println(F("Begin succeeded"));

    if (a0Volts > 0.2 || a1Volts > 0.2) {
      clearOutputsAndHalt(F("ZERO RESTORE FAIL: readings"));
    }
    Serial.println(F("Zero readings succeeded"));

    clearOutputsAndHalt(F("ZERO RESTORE PASS"));
  } else {
    if (!gp8403.begin()) {
      halt(F("UNKNOWN COMMAND: could not reset outputs"));
    }
    Serial.println(F("Begin succeeded"));

    clearOutputsAndHalt(F("UNKNOWN COMMAND"));
  }
}

void loop() {}

uint16_t readADC(uint16_t pin, uint8_t samples) {
  uint32_t total = 0;
  for (uint8_t i = 0; i < samples; i++) {
    total += analogRead(pin);
  }
  return total / samples;
}

char waitForCommand() {
  Serial.println(F("S=save 1V/2V, Z=verify and save zero, V=verify zero"));
  while (!Serial.available()) {
  }
  return Serial.read();
}

bool saveNVM() {
#ifndef ESP8266
  Wire.end();
#endif
  bool saved = gp8403.saveToNVM(SDA, SCL);
  Wire.begin();
  return saved;
}

void clearOutputsAndHalt(const __FlashStringHelper *message) {
  Serial.println(message);
  if (!gp8403.setVoltages(0.0, 0.0)) {
    Serial.println(F("NVM FAIL: reset outputs"));
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
