/*
  Dave Williams, DitroniX 2019-2023 (ditronix.net)
  GTEM-1 ATM90E26 Energy Monitoring Energy Monitor  v1.0
  Features include ESP32 GTEM ATM90E26 16bit ADC EEPROM OPTO CT-Clamp Current Voltage Frequency Power Factor GPIO I2C OLED SMPS D1 USB
  PCA 1.2212-105 - Test Code Firmware v1

  Full header information in main.cpp.

  This test code is OPEN SOURCE and formatted for easier viewing.  Although is is not intended for real world use, it may be freely used, or modified as needed.
  It is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

  Further information, details and examples can be found on our website wiki pages ditronix.net/wiki and also github.com/DitroniX
*/

// #include <GTEM-EEPROM.h>

// ****************  VARIABLES / DEFINES / STATIC / STRUCTURES / CONSTANTS ****************

// Variables
boolean CRCErrorFlag = false; // Updated to true if CRC error

// **************** FUNCTIONS / ROUTINES / CLASSES for CALIBRATION ****************

// Calibration Defaults.  If updated, CRC is autocalculated upon boot and re-read from EEPROM.
ATM90E26_SPI::ATM90E26_SPI(int pin)
{
  _cs = pin;
  _lgain = 0x1D39; // PL CONSTANT.  Use XLS to calculate these values. Examples: 0x1D39; 0x1D39
  _ugain = 0x9F9A; // VOLTAGE RMS.  Use XLS to calculate these values. Examples: 8V 0xA028 | 12V 0x9F9A or 0x9E38
  _igain = 0x2F6A; // CURRENT GAIN. Use XLS to calculate these values. Examples: 0x7160
}

// Register Defaults
void ATM90E26_SPI::InitEnergyIC()
{

  // Auto Calculated CRC and Restored from EEPROM.  If CRC Error - EEPROM is updated, ESP32 and ATM90E26 Restarted.
  _crc1 = readEEPROM16(0x1C);
  _crc2 = readEEPROM16(0x1E);

  // unsigned short systemstatus;
  pinMode(_cs, OUTPUT);

  /* Enable SPI */
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16);

  CommEnergyIC(0, SoftReset, 0x789A); // Perform soft reset

  CommEnergyIC(0, FuncEn, 0x0030); // Voltage sag irq=1, report on warnout pin=1, energy dir change irq=0
  CommEnergyIC(0, SagTh, 0x17DD);  // Voltage sag threshhold 0x1F2F

  // Set metering calibration values
  // CommEnergyIC(0, CalStart, 0x8765);  // RUNNING Metering calibration startup command. Register 21 to 2B need to be set
  CommEnergyIC(0, CalStart, 0x5678); // CAL Metering calibration startup command. Register 21 to 2B need to be set

  // Configure
  CommEnergyIC(0, MMode, 0x9422); // Metering Mode Configuration. All defaults. See pg 31 of datasheet.

  CommEnergyIC(0, PLconstH, 0x05CD); // PL Constant MSB 0x21 0x0525
  CommEnergyIC(0, PLconstL, 0xBB1C); // PL Constant LSB 0x22 0xFCB2

  CommEnergyIC(0, Lgain, _lgain);    // Line calibration gain 0x1D39
  CommEnergyIC(0, Lphi, 0x0000);     // Line calibration angle
  CommEnergyIC(0, PStartTh, 0x08BD); // Active Startup Power Threshold
  CommEnergyIC(0, PNolTh, 0x0000);   // Active No-Load Power Threshold
  CommEnergyIC(0, QStartTh, 0x0AEC); // Reactive Startup Power Threshold
  CommEnergyIC(0, QNolTh, 0x0000);   // Reactive No-Load Power Threshold
  CommEnergyIC(0, CSOne, _crc1);     // Write CSOne, as self calculated

  // Set measurement calibration values
  CommEnergyIC(0, AdjStart, 0x5678); // Measurement calibration startup command, registers 31-3A
  CommEnergyIC(0, Ugain, _ugain);    // Voltage rms gain
  CommEnergyIC(0, IgainL, _igain);   // L line current gain
  CommEnergyIC(0, Uoffset, 0x0000);  // Voltage offset
  CommEnergyIC(0, IoffsetL, 0x0000); // L line current offset
  CommEnergyIC(0, PoffsetL, 0x0000); // L line active power offset
  CommEnergyIC(0, QoffsetL, 0x0000); // L line reactive power offset
  CommEnergyIC(0, CSTwo, _crc2);     // Write CSTwo, as self calculated

  CommEnergyIC(0, CalStart, 0x8765); // Checks correctness of 21-2B registers and starts normal metering if ok
  CommEnergyIC(0, AdjStart, 0x8765); // Checks correctness of 31-3A registers and starts normal measurement  if ok

  Serial.println("");

  // Upon CRC Error - Update EEPROM with New Values and Auto Reboot
  if (GetSysStatus() & 0xC000 || GetSysStatus() & 0x3000)
  {

    Serial.println("Updating CRC Values in EEPROM");
    CRCErrorFlag = true;
    WriteEEPROM16(0x1C, CommEnergyIC(1, CSOne, 0x0000));
    WriteEEPROM16(0x1E, CommEnergyIC(1, CSTwo, 0x0000));
    CRCErrorFlag = false;
    
    Serial.println("\n####### Automatically Rebooting ESP32 and ATM90E26 #######\n");

    delay(3000);
    ESP.restart();
  }

  Serial.println("");
}