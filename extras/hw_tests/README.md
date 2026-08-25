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
