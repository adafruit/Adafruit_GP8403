/*
 * GP8403 QT Production Tester
 *
 * Runs automatically on the GP8403 QT Tester with an Arduino Uno. Verifies
 * the boosted 12 V rail, all eight I2C addresses, and both outputs in the 5 V
 * and 10 V ranges. Press the tester reset button to test the next board.
 */

#include <Adafruit_GP8403.h>

const uint16_t ADDRESS_0_PIN = 5;
const uint16_t ADDRESS_1_PIN = 4;
const uint16_t ADDRESS_2_PIN = 3;
const uint16_t PASS_LED_PIN = 12;
const uint16_t SPEAKER_PIN = 11;
const uint16_t VOUT0_ADC_PIN = A1;
const uint16_t VOUT1_ADC_PIN = A2;
const uint16_t BOOST_ADC_PIN = A3;

const uint8_t ADC_SAMPLES = 32;
const float BOOST_EXPECTED_VOLTS = 12.0;
const float BOOST_TOLERANCE_VOLTS = 1.0;

Adafruit_GP8403 gp8403;

void setup() {
  Serial.begin(115200);
  Serial.println(F("GP8403 QT production tester"));

  pinMode(PASS_LED_PIN, OUTPUT);
  digitalWrite(PASS_LED_PIN, LOW);
  pinMode(SPEAKER_PIN, OUTPUT);
  noTone(SPEAKER_PIN);

  pinMode(ADDRESS_0_PIN, OUTPUT);
  pinMode(ADDRESS_1_PIN, OUTPUT);
  pinMode(ADDRESS_2_PIN, OUTPUT);
  setAddressPins(0);
  delay(10);

  float boostVolts = readBoostVolts();
  Serial.print(F("Boost rail="));
  Serial.print(boostVolts, 3);
  Serial.println(F(" V"));
  if (abs(boostVolts - BOOST_EXPECTED_VOLTS) > BOOST_TOLERANCE_VOLTS) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: 12 V boost rail"));
  }
  Serial.println(F("12 V boost rail succeeded"));
  Serial.println();

  for (uint8_t addressOffset = 0; addressOffset < 8; addressOffset++) {
    setAddressPins(addressOffset);
    delay(1);

    uint8_t address = GP8403_DEFAULT_ADDRESS + addressOffset;
    if (!gp8403.begin(address)) {
      clearOutputsAndHalt(F("QT_TESTER FAIL: I2C address"));
    }
    Serial.print(F("I2C address 0x"));
    Serial.print(address, HEX);
    Serial.println(F(" succeeded"));
  }
  Serial.println();

  setAddressPins(0);
  if (!gp8403.begin()) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: restore default address"));
  }
  Serial.println(F("Default address restored"));
  Serial.println();

  if (!gp8403.setVoltages(1.0, 4.0)) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: set 1 V / 4 V"));
  }
  Serial.println(F("Set 1 V / 4 V succeeded"));

  delay(10);
  float vout0Volts = readOutputVolts(VOUT0_ADC_PIN);
  float vout1Volts = readOutputVolts(VOUT1_ADC_PIN);
  printOutputVoltages(vout0Volts, vout1Volts);
  if (abs(vout0Volts - 1.0) > 0.25 || abs(vout1Volts - 4.0) > 0.25) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: 1 V / 4 V readings"));
  }
  Serial.println(F("1 V / 4 V readings succeeded"));

  if (!gp8403.setVoltages(0.0, 0.0)) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: clear 1 V / 4 V"));
  }
  Serial.println(F("Clear 1 V / 4 V succeeded"));
  Serial.println();

  if (!gp8403.setOutputRange(GP8403_RANGE_10V)) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: select 10 V range"));
  }
  Serial.println(F("10 V range selection succeeded"));

  if (!gp8403.setVoltages(2.5, 7.5)) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: set 2.5 V / 7.5 V"));
  }
  Serial.println(F("Set 2.5 V / 7.5 V succeeded"));

  delay(10);
  vout0Volts = readOutputVolts(VOUT0_ADC_PIN);
  vout1Volts = readOutputVolts(VOUT1_ADC_PIN);
  printOutputVoltages(vout0Volts, vout1Volts);
  if (abs(vout0Volts - 2.5) > 0.35 || abs(vout1Volts - 7.5) > 0.35) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: 2.5 V / 7.5 V readings"));
  }
  Serial.println(F("2.5 V / 7.5 V readings succeeded"));

  if (!gp8403.setVoltages(0.0, 0.0)) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: clear 2.5 V / 7.5 V"));
  }
  Serial.println(F("Clear 2.5 V / 7.5 V succeeded"));
  Serial.println();

  if (!gp8403.setVoltages(9.0, 9.0)) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: set 9 V"));
  }
  Serial.println(F("Set 9 V succeeded"));

  delay(10);
  vout0Volts = readOutputVolts(VOUT0_ADC_PIN);
  vout1Volts = readOutputVolts(VOUT1_ADC_PIN);
  printOutputVoltages(vout0Volts, vout1Volts);
  if (abs(vout0Volts - 9.0) > 0.5 || abs(vout1Volts - 9.0) > 0.5) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: 9 V readings"));
  }
  Serial.println(F("9 V readings succeeded"));

  if (!gp8403.setOutputRange(GP8403_RANGE_5V)) {
    clearOutputsAndHalt(F("QT_TESTER FAIL: restore 5 V range"));
  }
  Serial.println(F("5 V range restore succeeded"));
  Serial.println();

  passAndHalt(F("QT_TESTER PASS"));
}

void loop() {}

void setAddressPins(uint8_t addressOffset) {
  digitalWrite(ADDRESS_0_PIN, bitRead(addressOffset, 0));
  digitalWrite(ADDRESS_1_PIN, bitRead(addressOffset, 1));
  digitalWrite(ADDRESS_2_PIN, bitRead(addressOffset, 2));
}

uint16_t readADC(uint16_t pin, uint8_t samples) {
  uint32_t total = 0;
  for (uint8_t i = 0; i < samples; i++) {
    total += analogRead(pin);
  }
  return total / samples;
}

float readOutputVolts(uint16_t pin) {
  // The 2:1 divider maps 0-1023 ADC counts to 0-10 V at the output.
  return map(readADC(pin, ADC_SAMPLES), 0, 1023, 0, 10000) / 1000.0;
}

float readBoostVolts() {
  // The 11:1 divider maps 0-1023 ADC counts to 0-55 V at the boost rail.
  return map(readADC(BOOST_ADC_PIN, ADC_SAMPLES), 0, 1023, 0, 55000) / 1000.0;
}

void printOutputVoltages(float vout0Volts, float vout1Volts) {
  Serial.print(F("VOUT0="));
  Serial.print(vout0Volts, 3);
  Serial.print(F(" V VOUT1="));
  Serial.print(vout1Volts, 3);
  Serial.println(F(" V"));
}

void resetOutputs() {
  setAddressPins(0);
  if (!gp8403.begin()) {
    Serial.println(F("QT_TESTER FAIL: reset begin"));
    return;
  }
  if (!gp8403.setVoltages(0.0, 0.0)) {
    Serial.println(F("QT_TESTER FAIL: reset outputs"));
  }
}

void clearOutputsAndHalt(const __FlashStringHelper *message) {
  Serial.println();
  Serial.println(message);
  resetOutputs();
  digitalWrite(PASS_LED_PIN, LOW);
  tone(SPEAKER_PIN, 220, 1000);

  while (true) {
    delay(1000);
  }
}

void passAndHalt(const __FlashStringHelper *message) {
  resetOutputs();
  Serial.println(message);
  Serial.println(F("Press reset to test the next board"));
  digitalWrite(PASS_LED_PIN, HIGH);
  tone(SPEAKER_PIN, 880, 100);
  delay(150);
  tone(SPEAKER_PIN, 1320, 250);

  while (true) {
    delay(1000);
  }
}
