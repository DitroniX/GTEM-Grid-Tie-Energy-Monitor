/*
  Dave Williams, DitroniX 2019-2022 (ditronix.net)
  GTEM-1 ATM90E26 Energy Monitoring Energy Monitor  v1.0
  Features include ESP32 GTEM ATM90E26 16bit ADC EEPROM OPTO CT-Clamp Current Voltage Frequency Power Factor GPIO I2C OLED SMPS D1 USB
  PCA 1.2212-104 - Test Code Firmware v1 - 23th December 2022

  The purpose of this test code is to cycle through the various main functions of the board, as shown below, as part of board bring up testing.
  Simplified Board Bring Up Test -GTEM ATM90E26 Energy Monitor ASIC - Basic Calibration Limits.  Requires Calibration.
  Additional diagnostic serial reporting has been included, for reference and expanded detail.

  Code Based on the excellent work from Tisham Dhar, whatnick | ATM90E26 Energy Monitor | Updated Date Williams

  Remember!
  Set the BOARD to ESP32, 'WEMOS D1 MINI ESP32' DEV Module (or similar).
  You may set the BAUD rate to 921600 to speed up flashing.
  The SDK does NOT need external power to flash.  It will take Power from the USB 5V.

  This test code is OPEN SOURCE and although is is not intended for real world use, it may be freely used, or modified as needed.
  It is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

  Further information, details and examples can be found on our website wiki pages ditronix.net/wiki and github.com/DitroniX
*/

// ****************  VARIABLES / DEFINES / STATIC / STRUCTURES / CONSTANTS ****************
// **************** FUNCTIONS / ROUTINES / CLASSES for CALIBRATION ****************

// Calibration Defaults
ATM90E26_SPI::ATM90E26_SPI(int pin) {
  _cs = pin;
  _lgain = 0x1D39;  // Use XLS to calculate these values. Examples: 0x1D39;
  _ugain = 0xA028;  // Use XLS to calculate these values. Examples: 0xD464;  0xA028;
  _igain = 0x7DFB;  // Use XLS to calculate these values. Examples: 0x6E49;  0x7DFB;
  _crc1 = 0xAE70;   // Run this application. Take auto calculated values and update here
  _crc2 = 0x4BE5;   // Run this application. Take auto calculated values and update here
}

// Register Defaults
void ATM90E26_SPI::InitEnergyIC() {
  unsigned short systemstatus;

  pinMode(_cs, OUTPUT);

  /* Enable SPI */
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16);

  CommEnergyIC(0, SoftReset, 0x789A);  // Perform soft reset

  CommEnergyIC(0, FuncEn, 0x0030);  // Voltage sag irq=1, report on warnout pin=1, energy dir change irq=0
  CommEnergyIC(0, SagTh, 0x17DD);   // Voltage sag threshhold 0x1F2F

  // Set metering calibration values
  //CommEnergyIC(0, CalStart, 0x8765);  // RUNNING Metering calibration startup command. Register 21 to 2B need to be set
  CommEnergyIC(0, CalStart, 0x5678);  // CAL Metering calibration startup command. Register 21 to 2B need to be set

  // Configure
  CommEnergyIC(0, MMode, 0x9422);  // Metering Mode Configuration. All defaults. See pg 31 of datasheet.

  CommEnergyIC(0, PLconstH, 0x05CD);  // PL Constant MSB 0x21 0x0525
  CommEnergyIC(0, PLconstL, 0xBB1C);  // PL Constant LSB 0x22 0xFCB2

  CommEnergyIC(0, Lgain, _lgain);     // Line calibration gain 0x1D39
  CommEnergyIC(0, Lphi, 0x0000);      // Line calibration angle
  CommEnergyIC(0, PStartTh, 0x08BD);  // Active Startup Power Threshold
  CommEnergyIC(0, PNolTh, 0x0000);    // Active No-Load Power Threshold
  CommEnergyIC(0, QStartTh, 0x0AEC);  // Reactive Startup Power Threshold
  CommEnergyIC(0, QNolTh, 0x0000);    // Reactive No-Load Power Threshold
  CommEnergyIC(0, CSOne, _crc1);      // Write CSOne, as self calculated

  // Set measurement calibration values
  CommEnergyIC(0, AdjStart, 0x5678);  // Measurement calibration startup command, registers 31-3A
  CommEnergyIC(0, Ugain, _ugain);     // Voltage rms gain
  CommEnergyIC(0, IgainL, _igain);    // L line current gain
  CommEnergyIC(0, Uoffset, 0x0000);   // Voltage offset
  CommEnergyIC(0, IoffsetL, 0x0000);  // L line current offset
  CommEnergyIC(0, PoffsetL, 0x0000);  // L line active power offset
  CommEnergyIC(0, QoffsetL, 0x0000);  // L line reactive power offset
  CommEnergyIC(0, CSTwo, _crc2);      // Write CSTwo, as self calculated

  CommEnergyIC(0, CalStart, 0x8765);  //Checks correctness of 21-2B registers and starts normal metering if ok
  CommEnergyIC(0, AdjStart, 0x8765);  //Checks correctness of 31-3A registers and starts normal measurement  if ok

  Serial.println("");

  // Checksums - Displayed on Power Up.  See Calibration Defaults. systemstatus = GetSysStatus();
  if (systemstatus & 0xC000) {  // Checksum 1 Error
    Serial.println("");
    Serial.print("Checksum 1 Error!! Currently: 0x");
    Serial.print(_crc1, HEX);
    Serial.print("\tPlease change _crc1 to: 0x");
    Serial.println(CommEnergyIC(1, CSOne, 0x0000), HEX);
  }
  if (systemstatus & 0x3000) {  // Checksum 2 Error
    Serial.println("");
    Serial.print("Checksum 2 Error!! Currently: 0x");
    Serial.print(_crc2, HEX);
    Serial.print("\tPlease change _crc2 to: 0x");
    Serial.println(CommEnergyIC(1, CSTwo, 0x0000), HEX);
  }

  Serial.println("");
}