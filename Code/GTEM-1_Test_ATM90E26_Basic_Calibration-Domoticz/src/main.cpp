/*
  Dave Williams, DitroniX 2019-2023 (ditronix.net)
  GTEM-1 ATM90E26 Energy Monitoring Energy Monitor  v1.0
  Features include ESP32 GTEM ATM90E26 16bit ADC EEPROM OPTO CT-Clamp Current Voltage Frequency Power Factor GPIO I2C OLED SMPS D1 USB
  PCA 1.2212-105 - Test Code Firmware v1

  The purpose of this test code is to cycle through the various main functions of the board, as shown below, as part of board bring up testing.

  Simplified Board Bring Up Test -GTEM ATM90E26 Energy Monitor ASIC - Basic Calibration Limits.  Requires Calibration.
  Additional diagnostic serial reporting has been included, for reference and expanded detail.
  Three Modes:
    1 Calibration One upon Reset
     - User Button can be used to force single update the Display Registers.
    2 Domoticz Publish (Loop)

  Instructions.  See GitHub.com/DitroniX or DitroniX.net/Wiki for further information.

    - First Flash this code to a GTEM board and Run code.
    - Check the Mains Current and Voltage display on the Serial Monitor - Press board Reset to refresh data.
    - You should find that the values are pretty near what is expected i.e. voltage, current, power etc.
      - If not, update values, where needed, in the Excel 'Energy Setpoint Calculator GTEM Bring-Up Only.xlsx'.  Typically ONLY UGain or iGain.
      - Enter new/tweaked UGain (Voltage) and/or iGain (Current).
      - Update auto calculated Hex value(s) into 'GTEM-1_Defaults.h' > 'Calibration Defaults'.
      - Reflash code to board.
      - The Red LED will Flash upon a CRC1 or CRC2 error and then AUTOMATICALLY update the EEPROM and Reboot.
      - You should see a change in the values for Current, Voltage and resultant Power (Wattage).
      - Go back to XLS and update until you are happy that the values are near to your expected actual readings.
    - Update the Wifi, Domoticz Server and Device Index Values in 'Domoticz.h'.  Creating new Devices first in Domoticz.
    - Once you are happy with the values, update the 'EnableDomoticz' to 'true'.
    - Reflash code to board.  All done!

  Code register formulation based on the excellent ground work from Tisham Dhar, whatnick | ATM90E26 Energy Monitor | Code upgraded and updated by Date Williams

  Remember!
  Set the BOARD to ESP32, 'WEMOS D1 MINI ESP32' DEV Module (or similar).
  You may set the BAUD rate to 921600 to speed up flashing.
  The SDK does NOT need external power to flash.  It will take Power from the USB 5V.

  Note: In the default state, upon first power up and during reset, the Green LED may be partially lit. Once programmed and the GPIO defined, the Green LED will go off after power up.

  This test code is OPEN SOURCE and formatted for easier viewing.  Although is is not intended for real world use, it may be freely used, or modified as needed.
  It is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

  Further information, details and examples can be found on our website wiki pages ditronix.net/wiki and also github.com/DitroniX
*/

// Libraries
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <driver/adc.h>
#include <GTEM-EEPROM.h>
#include <EnergyATM90E26.h>
#include <GTEM-1_Defaults.h>
#include <Domoticz.h>

// ****************  VARIABLES / DEFINES / STATIC / STRUCTURES ****************

// App
String AppVersion = "GTEM Test 230125";
String AppBuild = "DitroniX GTEM-1 ATM90E26 SDK Board";
String AppName = "GTEM Energy Monitor - Calibration and Domoticz";

// Variables
unsigned short ReadValue;
float ReadFloat;
float ADC_Voltage;
float TemperatureC;
float TemperatureF;

// Constants
const int LoopDelay = 1;     // Loop Delay in Seconds
float ADC_Constant = 31.340; // Adjust as needed for calibration of VDC_IN.
uint64_t chipid = ESP.getEfuseMac();

// **************** INPUTS ****************
#define DCV_IN 36      // GPIO 36 (Analog VP / ADC 1 CH0)
#define NTC_IN 39      // GPIO 39/VN (Analog ADC 1 CH3)
#define User_Button 26 // GPIO 26 (DAC2)
#define User_Jumper 25 // GPIO 25 (DAC1)
#define OPTO_IN 33     // GPIO 33 (Digital ADC 1 CH5)
#define ATM_WO 27      // GPIO 28 (Digital ADC 2 CH7)
#define ATM_CF1 34     // GPIO 34 (Digital ADC 1 CH6)
#define ATM_CF2 35     // GPIO 35 (Digital ADC 1 CH7)

// **************** OUTPUTS ****************
#define LED_Red 2   // Red LED
#define LED_Green 4 // Green LED
#define LED_Blue 15 // Blue LED

// **************** GPIO ****************
#define USR_GP12 12 // GPIO 12 (Digital TBC)
#define USR_GP14 14 // GPIO 14 (Digital TBC)

// Define I2C (Expansion Port)
#define I2C_SDA 21
#define I2C_SCL 22

// ######### OBJECTS #########
ATM90E26_SPI eic;

// **************** FUNCTIONS AND ROUTINES ****************

void DisplayBIN16(int var) // Display BIN from Var
{
  for (unsigned int i = 0x8000; i; i >>= 1)
  {
    Serial.write(var & i ? '1' : '0');
  }
  Serial.print(" ");
}

void DisplayHEX(unsigned long var, unsigned char numChars) // Display Hex from Var
{
  unsigned long mask = 0x0000000F;
  mask = mask << 4 * (numChars - 1);

  for (unsigned int i = numChars; i > 0; --i)
  {
    Serial.print(((var & mask) >> (i - 1) * 4), HEX);
    mask = mask >> 4;
  }
  Serial.print(" ");
}

void DisplayRegisters() // Display Diagnostic Report
{
  // Heatbeat Green LED and Provide ATM Stablising Time
  digitalWrite(LED_Green, LOW);
  delay(250);
  digitalWrite(LED_Green, HIGH);
  delay(250);
  digitalWrite(LED_Green, LOW);
  delay(250);
  digitalWrite(LED_Green, HIGH);

  // Header
  Serial.println("GTEM-1 ATM90E26 Energy Monitoring Energy Monitor");
  Serial.println("Register Status and Diagnostic Report");
  Serial.printf("ESP32 Serial ID = %04X", (uint16_t)(chipid >> 32));
  Serial.printf("%08X", (uint32_t)chipid);
  Serial.print("   Firmware Version = ");
  Serial.println(AppVersion);
  Serial.println();
  Serial.println("Register Name\t\t\tVar/Address\t\tValue / Binary / Information");
  Serial.println("------------ \t\t\t-----------\t\t--------------------------------------------------------");

  // System Status
  Serial.print("System Status \t\t\t(SysStatus 0x01):\t0x");
  ReadValue = eic.GetSysStatus();
  DisplayHEX(ReadValue, 4);
  DisplayBIN16(ReadValue);
  if (bitRead(ReadValue, 1))
    Serial.print("SagWarn.Enabled ");
  if (bitRead(ReadValue, 13) && bitRead(ReadValue, 12))
    Serial.print("CheckSumError.CS2 ");
  if (bitRead(ReadValue, 15) && bitRead(ReadValue, 14))
    Serial.print("CheckSumError.CS1 ");
  Serial.println();
  if (ReadValue == 0x0000)
    Serial.println(">ATM 0x01 - #0000 System Status Default Value");
  if (ReadValue == 0xFFFF)
    Serial.println(">ATM 0x01 - #FFFF Failed | Fault on ATM | Reboot Needed");

  // Meter Status
  yield();
  Serial.print("Meter Status \t\t\t(EnStatus 0x46):\t0x");
  ReadValue = eic.GetMeterStatus();
  DisplayHEX(ReadValue, 4);
  DisplayBIN16(ReadValue);
  if (!bitRead(ReadValue, 1) && !bitRead(ReadValue, 0))
    Serial.print("LNMode.AntiTamper ");
  if (!bitRead(ReadValue, 1) && bitRead(ReadValue, 0))
    Serial.print("LNMode.FixedL ");
  if (bitRead(ReadValue, 1) && !bitRead(ReadValue, 0))
    Serial.print("LNMode.LN ");
  if (bitRead(ReadValue, 1) && bitRead(ReadValue, 0))
    Serial.print("LNMode.Flexible ");
  if (bitRead(ReadValue, 11))
    Serial.print("Lline.AntiTamperL ");
  if (!bitRead(ReadValue, 11))
    Serial.print("Lline.AntiTamperN ");
  if (bitRead(ReadValue, 12))
    Serial.print("RevP.CF1ActiveReverse ");
  if (!bitRead(ReadValue, 12))
    Serial.print("RevP.CF1ActiveForward ");
  if (bitRead(ReadValue, 13))
    Serial.print("RevQ.CF2ReActiveReverse ");
  if (!bitRead(ReadValue, 13))
    Serial.print("RevQ.CF2ReActiveForward ");
  if (bitRead(ReadValue, 14))
    Serial.print("Pnoload.NoLoadActive ");
  if (!bitRead(ReadValue, 14))
    Serial.print("Pnoload.NoLoadNotActive ");
  if (bitRead(ReadValue, 15))
    Serial.print("Qnoload.ReactiveNoLoad ");
  if (!bitRead(ReadValue, 15))
    Serial.print("Qnoload.NotReactiveNoLoad ");
  Serial.println();
  if (ReadValue == 0x2801)
    Serial.println(">ATM 0x46- #2801 Accumulator Populated");
  if (ReadValue == 0xC801)
    Serial.println(">ATM 0x46- #C801 Accumulator Not Running");
  if (ReadValue == 0xC800)
    Serial.println(">ATM 0x46 - #C800 Meter Status Default Value");
  if (ReadValue == 0xFFFF)
    Serial.println(">ATM 0x46 - #FFFF Failed | Fault on ATM | Reboot Needed");
  if (ReadValue == 0x0000)
    Serial.println(">ATM 0x46 - #0000 ERROR!: Possible ATM Hardware Issue\n\n");

  // MMode Metering Status
  yield();
  Serial.print("MMode Status \t\t\t(MMode 0x2B):\t\t0x");
  ReadValue = eic.GetMModeStatus();
  DisplayHEX(ReadValue, 4);
  DisplayBIN16(ReadValue);
  if (!bitRead(ReadValue, 5) && !bitRead(ReadValue, 4))
    Serial.print("MMode.PositiveZeroCrossing ");
  if (!bitRead(ReadValue, 5) && bitRead(ReadValue, 4))
    Serial.print("MMode.NegativeZeroCrossing ");
  if (bitRead(ReadValue, 5) && !bitRead(ReadValue, 4))
    Serial.print("MMode.AllZeroCrossing ");
  if (bitRead(ReadValue, 5) && bitRead(ReadValue, 4))
    Serial.print("MMode.NoZeroCrossing ");
  if (bitRead(ReadValue, 10))
    Serial.print("MMode.LNSel.LLine(Default) ");
  if (!bitRead(ReadValue, 10))
    Serial.print("MMode.LNSel.NLine ");
  if (!bitRead(ReadValue, 12) && !bitRead(ReadValue, 11))
    Serial.print("MMode.NLine.CurrentGain2 ");
  if (!bitRead(ReadValue, 12) && bitRead(ReadValue, 11))
    Serial.print("MMode.NLine.CurrentGain4 ");
  if (bitRead(ReadValue, 12) && !bitRead(ReadValue, 11))
    Serial.print("MMode.NLine.CurrentGain1 ");
  if (bitRead(ReadValue, 12) && bitRead(ReadValue, 11))
    Serial.print("MMode.NLine.CurrentGain1 ");
  if (bitRead(ReadValue, 15))
    Serial.print("MMode.CurrentChannelGain1 ");
  if (!bitRead(ReadValue, 15) && !bitRead(ReadValue, 14) && !bitRead(ReadValue, 13))
    Serial.print("MMode.LGain.CurrentChannelGain4 ");
  if (!bitRead(ReadValue, 15) && !bitRead(ReadValue, 14) && bitRead(ReadValue, 13))
    Serial.print("MMode.LGain.CurrentChannelGain8 ");
  if (!bitRead(ReadValue, 15) && bitRead(ReadValue, 14) && !bitRead(ReadValue, 13))
    Serial.print("MMode.LGain.CurrentChannelGain16 ");
  if (!bitRead(ReadValue, 15) && bitRead(ReadValue, 14) && bitRead(ReadValue, 13))
    Serial.print("MMode.LGain.CurrentChannelGain24 ");
  Serial.println();
  if (ReadValue == 0x9422)
    Serial.println(">ATM 0x2B - #9422 MMode Default Value");

  // ATM Read Values

  Serial.println("-----------");

  // CalStart Status
  yield();
  Serial.print("Calibraration Status \t\t(CalStart 0x20):\t0x");
  ReadValue = eic.GetCalStartStatus();
  DisplayHEX(ReadValue, 4);
  DisplayBIN16(ReadValue);
  if (ReadValue == 0x6886)
    Serial.print("Power-On Value. Metering Function is Disabled");
  if (ReadValue == 0x5678)
    Serial.print("CALIBRATION | Meter Calibration Startup Command");
  if (ReadValue == 0x8765)
    Serial.print("RUNNING | Normal Metering Mode");
  if (ReadValue != 0x6886 && ReadValue != 0x5678 && ReadValue != 0x8765)
    Serial.print(">ATM 0x20 - Metering Function is Disabled");
  Serial.println();

  yield();
  Serial.print("UGain Calibration Value\t\t(UGain 0x31):\t\t0x");
  ReadValue = eic.GetUGain();
  DisplayHEX(ReadValue, 4);
  if (bitRead(ReadValue, 15))
    Serial.print("UGain Possible Value Error");
  Serial.println();

  yield();
  Serial.print("LGain Calibration Value\t\t(LGain 0x23):\t\t0x");
  ReadValue = eic.GetLGain();
  DisplayHEX(ReadValue, 4);
  Serial.println();

  yield();
  Serial.print("IGain Calibration Value\t\t(IgainL 0x32):\t\t0x");
  ReadValue = eic.GetIGain();
  DisplayHEX(ReadValue, 4);
  Serial.println();

  // Checksum 1 Status
  yield();
  Serial.print("Checksum Status \t\t(CS1 0x2C):\t\t0x");
  ReadValue = eic.GetCS1Status();
  DisplayHEX(ReadValue, 4);
  if (ReadValue != eic.GetCS1Calculated())
  { // 0xC000
    Serial.print("*ERROR: Please update _crc1 to ATM Calculated CRC: 0x");
    Serial.print(eic.GetCS1Calculated(), HEX);
  }
  Serial.println();

  // Checksum 2 Status
  yield();
  Serial.print("Checksum Status \t\t(CS2 0x3B):\t\t0x");
  ReadValue = eic.GetCS2Status();
  DisplayHEX(ReadValue, 4);
  if (ReadValue != eic.GetCS2Calculated())
  { // 0x3000
    Serial.print("*ERROR: Please update _crc2 to ATM Calculated CRC: 0x");
    Serial.print(eic.GetCS2Calculated(), HEX);
  }
  Serial.println();

  Serial.println("-----------");

  yield();
  Serial.print("Line Voltage \t\t\t(Urms 0x49):\t\t");
  Serial.print(eic.GetLineVoltage());
  Serial.println(" V");

  yield();
  Serial.print("Line Current \t\t\t(Irms 0x48):\t\t");
  Serial.print(eic.GetLineCurrent());
  Serial.println(" A");

  yield();
  Serial.print("Line Frequency \t\t\t(Freq 0x4C):\t\t");
  Serial.print(eic.GetFrequency());
  Serial.println(" Hz");

  yield();
  ReadFloat = eic.GetActivePower();
  Serial.print("Active Power \t\t\t(Pmean 0x4A):\t\t");
  Serial.print(ReadFloat);
  Serial.println(" W");

  yield();
  ReadFloat = eic.GetImportPower();
  Serial.print("Import Power \t\t\t(Pmean 0x4A +):\t\t");
  Serial.print(ReadFloat);
  Serial.println(" W");

  yield();
  ReadFloat = eic.GetExportPower();
  Serial.print("Export Power \t\t\t(Pmean 0x4A -):\t\t");
  Serial.print(ReadFloat);
  Serial.println(" W");

  Serial.println("-----------");

  yield();
  Serial.print("Import Energy \t\t\t(APenergy 0x40):\t");
  Serial.println(eic.GetImportEnergy());

  yield();
  Serial.print("Export Energy \t\t\t(ANenergy 0x41):\t");
  Serial.println(eic.GetExportEnergy());

  yield();
  Serial.print("Power Factor \t\t\t(PowerF 0x4D):\t\t");
  Serial.println(eic.GetPowerFactor());

  Serial.println("-----------");

  yield();
  Serial.print("Abs Active Energy \t\t(ATenergy 0x42):\t");
  Serial.println(eic.GetAbsActiveEnergy());

  yield();
  Serial.print("Abs Reactive Energy \t\t(Rtenergy 0x45):\t");
  Serial.println(eic.GetAbsReactiveEnergy());

  yield();
  Serial.print("Abs Reactive Forward Energy \t(RPenergy 0x43):\t");
  Serial.println(eic.GetReactivefwdEnergy());

  // LSB RMS/Power Status
  yield();
  Serial.print("LSB RMS/Power \t\t\t(LSB 0x08):\t\t0x");
  ReadValue = eic.GetLSBStatus();
  DisplayHEX(ReadValue, 4);
  Serial.println(ReadValue);

  // Other GTEM Sensors

  // ESP32 ADC 12-Bit SAR (Successive Approximation Register)
  // Conversion resolution 0 - 4095 (4096)
  // You may need to calibrate as needed.

  Serial.println("-----------");

  // DCV_IN
  yield();
  // ReadADCVoltage();
  Serial.print("DC Voltage Sensor \t\t(DCV_IN VP):\t\t");
  Serial.print(ADC_Voltage);
  if (ADC_Voltage < 5)
    Serial.print(" V USB Powered.  Note - Not all ATM functions will work in this mode");
  if (ADC_Voltage > 5)
    Serial.print(" V AC/DC Input");
  if (ADC_Voltage > 20)
    Serial.print(" V *WARNING: Please Check Input Voltage.  Too High!");
  Serial.println();

  // NTC
  yield();
  // ReadTemperature();
  Serial.print("PCB Temperature Sensor\t\t(NTC_IN VN):\t\t");
  Serial.print(TemperatureC);
  Serial.println(" ºC");

  Serial.println("\n");
}

void TestRGB()
{ // Test RGB LEDs

  Serial.println("Testing RGB LED");
  delay(100);

  // Red
  digitalWrite(LED_Red, LOW);
  delay(500);
  digitalWrite(LED_Red, HIGH);
  delay(100);

  // Green
  digitalWrite(LED_Green, LOW);
  delay(500);
  digitalWrite(LED_Green, HIGH);
  delay(100);

  // Blue
  digitalWrite(LED_Blue, LOW);
  delay(500);
  digitalWrite(LED_Blue, HIGH);
  delay(100);

  // White
  digitalWrite(LED_Red, LOW);
  digitalWrite(LED_Green, LOW);
  digitalWrite(LED_Blue, LOW);
  delay(500);
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Green, HIGH);
  digitalWrite(LED_Blue, HIGH);
}

void PublishRegisters()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    // ATM90E26 Registers
    yield();
    if (LineVoltage > 0)
    {
      ReadFloat = eic.GetLineVoltage();
      PublishDomoticz(LineVoltage, ReadFloat, "LineVoltage");
      yield();
    }

    if (LineCurrent > 0)
    {
      ReadFloat = eic.GetLineCurrent();
      PublishDomoticz(LineCurrent, ReadFloat, "LineCurrent");
      yield();
    }

    if (ActivePower > 0)
    {
      ReadFloat = eic.GetActivePower();
      PublishDomoticz(ActivePower, ReadFloat, "ActivePower");
      yield();
    }

    if (ImportPower > 0)
    {
      ReadFloat = eic.GetImportPower();
      PublishDomoticz(ImportPower, ReadFloat, "ImportPower");
      yield();
    }

    if (ExportPower > 0)
    {
      ReadFloat = eic.GetExportPower();
      PublishDomoticz(ExportPower, ReadFloat, "ExportPower");
      yield();
    }

    if (LineFrequency > 0)
    {
      ReadFloat = eic.GetFrequency();
      PublishDomoticz(LineFrequency, ReadFloat, "LineFrequency");
      yield();
    }

    if (ImportEnergy > 0)
    {
      ReadFloat = eic.GetImportEnergy();
      PublishDomoticz(ImportEnergy, ReadFloat, "ImportEnergy");
      yield();
    }

    if (ExportEnergy > 0)
    {
      ReadFloat = eic.GetExportEnergy();
      PublishDomoticz(ExportEnergy, ReadFloat, "ExportEnergy");
      yield();
    }

    if (PowerFactor > 0)
    {
      ReadFloat = eic.GetPowerFactor();
      PublishDomoticz(PowerFactor, ReadFloat, "PowerFactor");
      yield();
    }

    // ReadADCVoltage();
    if (DCVoltage > 0)
      PublishDomoticz(DCVoltage, ADC_Voltage, "DCVoltage");
    yield();

    // ReadTemperature();
    if (PCBTemperature > 0)
      PublishDomoticz(PCBTemperature, TemperatureC, "PCBTemperature");

    // Batch or Group Device
    if (DomoticzBaseIndex > 0)
      PublishDomoticzATM(DomoticzBaseIndex);
  }
  else
  {
    InitialiseWiFi();
  }

  Serial.println("");
}

void ScanI2CBus()
{ // I2C Bus Scanner

  byte error, address;
  int nDevices;

  Serial.println("Scanning I2C Bus for Devices ...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address Decimal ");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.print(address);
      Serial.print(" = Hex 0x");
      Serial.print(address, HEX);
      if (address == 80)
        Serial.print(" EEPROM");
      Serial.println();

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address Decimal ");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.print(address);
      Serial.print(" = Hex 0x");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
  {
    Serial.println("No I2C devices found. Possible Hardware Issue?");
  }
  else
  {
    Serial.println("I2C Bus Scan Complete");
  }
}

void ReadTemperature()
{
  int Vo;
  float R1 = 10000; // Based on 10K
  float logR2, R2, T;
  // float Tf;
  float tCal = 1.16; // Tweak for Calibration
  float C1 = 1.009249522e-03, C2 = 2.378405444e-04, C3 = 2.019202697e-07;

  Vo = analogRead(NTC_IN);
  R2 = R1 * (4095.0 / (float)Vo - tCal);
  logR2 = log(R2);
  T = (1.0 / (C1 + C2 * logR2 + C3 * logR2 * logR2 * logR2));
  TemperatureC = T - 273.15;                        // Centigrade
  TemperatureF = (TemperatureC * 9.0) / 3.3 + 32.0; // Fahrenheit
}

void ReadADCVoltage()
{
  ADC_Voltage = (analogRead(DCV_IN) * ADC_Constant) / 4095;
}

// **************** SETUP ****************
void setup()
{

  // Stabalise
  delay(250);

  // Initialise UART
  Serial.begin(115200, SERIAL_8N1); // 115200
  while (!Serial)
    ;
  Serial.println("");

  // Application Info
  Serial.println("");
  Serial.println(AppVersion + " Initialized");
  Serial.println(AppBuild);
  Serial.println(AppName);
  Serial.println("GTEM Hardware Setup and Power-Up Test");
  Serial.println("");

  // LEDs
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Green, OUTPUT);
  pinMode(LED_Blue, OUTPUT);

  // LEDs Default Off State
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Green, HIGH);
  digitalWrite(LED_Blue, HIGH);

  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Hardware Tests
  TestRGB();          // Cycle RGB LED
  ScanI2CBus();       // Scan I2C Bus and Report Devices
  InitializeEEPROM(); // Initialize EEPROM

  /*Initialise ATM90E26 + SPI port */
  eic.InitEnergyIC();

  // Stabalise
  delay(250);

  ReadTemperature(); // Read PCB NTC Temperature
  ReadADCVoltage();  // Read AC>DC Input Voltage

  DisplayRegisters(); // Display Registers Once.  Update CRC if required and store in EEPROM.
}

// **************** LOOP ****************
void loop()
{

  // LED Red Flashing if CRC Error
  if (CRCErrorFlag)
  {
    digitalWrite(LED_Red, HIGH);
    delay(200);
    digitalWrite(LED_Red, LOW);
    delay(200);
  }
  else
  {
    if (EnableDomoticz == true)
    {
      // PublishRegisters
      ReadTemperature();  // Read PCB NTC Temperature
      ReadADCVoltage();   // Read AC>DC Input Voltage
      PublishRegisters(); // Publish to Domoticz
    }
    // Heatbeat LED
    digitalWrite(LED_Blue, LOW);
    delay(50);
    digitalWrite(LED_Blue, HIGH);

    // Loop Delay
    delay(LoopDelay * 1000);
  }
}
