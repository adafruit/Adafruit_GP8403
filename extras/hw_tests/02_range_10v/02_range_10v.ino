/*
 * GP8403 10 V Range Hardware Test
 *
 * Verifies several output voltages through equal 10 kOhm dividers on A0/A1.
 */

#include <Adafruit_GP8403.h>

Adafruit_GP8403 gp8403;

void setup() {
  Serial.begin(115200);
  Serial.println(F("GP8403 10 V range hardware test"));

  if (!gp8403.begin()) {
    halt(F("RANGE_10V FAIL: begin"));
  }
  Serial.println(F("Begin succeeded"));

  if (!gp8403.setOutputRange(GP8403_RANGE_10V)) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: select 10 V range"));
  }
  Serial.println(F("10 V range selection succeeded"));

  if (!gp8403.setVoltages(2.5, 7.5)) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: set 2.5 V / 7.5 V"));
  }
  Serial.println(F("Set 2.5 V / 7.5 V succeeded"));

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
    clearOutputsAndHalt(F("RANGE_10V FAIL: clear 2.5 V / 7.5 V"));
  }
  Serial.println(F("Clear 2.5 V / 7.5 V succeeded"));

  if (abs(a0Volts - 2.5) > 0.35 || abs(a1Volts - 7.5) > 0.35) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: 2.5 V / 7.5 V readings"));
  }
  Serial.println(F("2.5 V / 7.5 V readings succeeded"));

  if (!gp8403.setVoltages(9.0, 9.0)) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: set 9 V"));
  }
  Serial.println(F("Set 9 V succeeded"));

  delay(10);
  a0Volts = map(readADC(A0, 32), 0, 1023, 0, 10000) / 1000.0;
  a1Volts = map(readADC(A1, 32), 0, 1023, 0, 10000) / 1000.0;
  Serial.print(F("A0="));
  Serial.print(a0Volts, 3);
  Serial.print(F(" V A1="));
  Serial.print(a1Volts, 3);
  Serial.println(F(" V"));
  if (!gp8403.setVoltages(0.0, 0.0)) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: clear 9 V"));
  }
  Serial.println(F("Clear 9 V succeeded"));

  if (abs(a0Volts - 9.0) > 0.35 || abs(a1Volts - 9.0) > 0.35) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: 9 V readings"));
  }
  Serial.println(F("9 V readings succeeded"));

  if (!gp8403.setVoltages(10.0, 10.0)) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: set 10 V"));
  }
  Serial.println(F("Set 10 V succeeded"));

  delay(10);
  a0Volts = map(readADC(A0, 32), 0, 1023, 0, 10000) / 1000.0;
  a1Volts = map(readADC(A1, 32), 0, 1023, 0, 10000) / 1000.0;
  Serial.print(F("A0="));
  Serial.print(a0Volts, 3);
  Serial.print(F(" V A1="));
  Serial.print(a1Volts, 3);
  Serial.println(F(" V"));
  if (!gp8403.setVoltages(0.0, 0.0)) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: clear 10 V"));
  }
  Serial.println(F("Clear 10 V succeeded"));

  if (abs(a0Volts - 10.0) > 0.5 || abs(a1Volts - 10.0) > 0.5) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: 10 V readings"));
  }
  Serial.println(F("10 V readings succeeded"));

  if (!gp8403.setOutputRange(GP8403_RANGE_5V)) {
    clearOutputsAndHalt(F("RANGE_10V FAIL: restore 5 V range"));
  }
  Serial.println(F("5 V range restore succeeded"));

  clearOutputsAndHalt(F("RANGE_10V PASS"));
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
    Serial.println(F("RANGE_10V FAIL: reset outputs"));
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
