# GP8403 hardware tests

Each folder contains one self-contained sketch. No shared test headers or
captured output files are required.

The fixture uses an Adafruit Metro Mini with the GP8403 connected to its I2C SDA
and SCL pins and a shared ground. OUT0 connects through an equal 10 kOhm / 10
kOhm divider to A0, and OUT1 connects through another equal 10 kOhm / 10 kOhm
divider to A1. The tests assume a 5 V ADC reference. Run `00_basic`,
`01_invalid_inputs`, and `02_range_10v` directly, in that order.

For `03_nvm_persistence`, open the serial monitor at 115200 baud and follow this
three-boot sequence:

1. Send `S` to save 1 V / 2 V, then remove and restore all power.
2. Send `Z` to verify 1 V / 2 V and save zero, then power cycle again.
3. Send `V` to verify that zero was restored.

`04_qt_tester` is the automatic production test for the GP8403 QT Tester PCB
and an Arduino Uno. The fixture drives the DUT address pins from D5, D4, and D3;
measures VOUT0 and VOUT1 on A1 and A2 through equal 10 kOhm dividers; measures
the boosted 12 V rail on A3 through a 10 kOhm / 1 kOhm divider; drives the green
pass LED from D12; and drives the buzzer from D11. Press the tester reset button
to test the next board. This test intentionally does not write NVM.
