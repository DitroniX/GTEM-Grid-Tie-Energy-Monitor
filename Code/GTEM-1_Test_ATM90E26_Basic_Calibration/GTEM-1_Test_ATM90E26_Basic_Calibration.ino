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
  
  Note: In the default state, upon first power up and during reset, the Green LED may be partially lit. Once programmed and the GPIO defined, the Green LED will go off after power up.

  This test code is OPEN SOURCE and formatted for easier viewing.  Although is is not intended for real world use, it may be freely used, or modified as needed.
  It is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

  Further information, details and examples can be found on our website wiki pages ditronix.net/wiki and github.com/DitroniX
*/

// Libraries
#include <Arduino.h>
#include <SPI.h>
#include <driver/adc.h>
#include "EnergyATM90E26.h"
#include "GTEM-1_Defaults.h"

// ****************  VARIABLES / DEFINES / STATIC / STRUCTURES ****************

// Variables
unsigned short s_status;
float ADC_Voltage;
float ADC_Constant;

// Constants
const int LoopDelay = 1;  // Loop Delay in Seconds
uint64_t chipid = ESP.getEfuseMac();

// **************** INPUTS ****************
#define DCV_IN 36  //GPIO 36 (Analog VP / ADC 1 CH0)
#define NTC_IN 39  //GPIO 39/VN (Analog ADC 1 CH3)

// **************** OUTPUTS ****************
#define LED_Red 2    // Red LED
#define LED_Green 4  // Green LED
#define LED_Blue 15 // Blue LED

ATM90E26_SPI eic;

// **************** FUNCTIONS AND ROUTINES ****************

void DisplayBIN16(int var) {
  for (unsigned int i = 0x8000; i; i >>= 1) {
    Serial.write(var & i ? '1' : '0');
  }
  Serial.print(" ");
}

void DisplayHEX(unsigned long var, unsigned char numChars) {
  unsigned long mask = 0x0000000F;
  mask = mask << 4 * (numChars - 1);

  for (unsigned int i = numChars; i > 0; --i) {
    Serial.print(((var & mask) >> (i - 1) * 4), HEX);
    mask = mask >> 4;
  }
  Serial.print(" ");
}

void DisplayRegisters() {  // Display Diagnostic Report

  // Header
  Serial.println("GTEM-1 ATM90E26 Energy Monitoring Energy Monitor - Register Status and Diagnostic Report");
  Serial.printf("ESP32 Serial ID = %04X", (uint16_t)(chipid >> 32));
  Serial.printf("%08X\n", (uint32_t)chipid);
  Serial.println();
  Serial.println("Register Name\t\t\tVar/Address\t\tValue / Binary / Information");
  Serial.println("------------ \t\t\t-----------\t\t--------------------------------------------------------");

  // System Status
  Serial.print("System Status \t\t\t(SysStatus 0x01):\t0x");
  s_status = eic.GetSysStatus();
  DisplayHEX(s_status, 4);
  DisplayBIN16(s_status);
  if (bitRead(s_status, 1)) Serial.print("SagWarn.Enabled ");
  if (bitRead(s_status, 13) && bitRead(s_status, 12)) Serial.print("CheckSumError.CS2 ");
  if (bitRead(s_status, 15) && bitRead(s_status, 14)) Serial.print("CheckSumError.CS1 ");
  Serial.println();
  if (s_status == 0x0000) Serial.println(">ATM 0x01 - #0000 System Status Default Value");
  if (s_status == 0xFFFF) Serial.println(">ATM 0x01 - #FFFF Failed | Fault on ATM | Reboot Needed");

  // Meter Status
  yield();
  Serial.print("Meter Status \t\t\t(EnStatus 0x46):\t0x");
  s_status = eic.GetMeterStatus();
  DisplayHEX(s_status, 4);
  DisplayBIN16(s_status);
  if (!bitRead(s_status, 1) && !bitRead(s_status, 0)) Serial.print("LNMode.AntiTamper ");
  if (!bitRead(s_status, 1) && bitRead(s_status, 0)) Serial.print("LNMode.FixedL ");
  if (bitRead(s_status, 1) && !bitRead(s_status, 0)) Serial.print("LNMode.LN ");
  if (bitRead(s_status, 1) && bitRead(s_status, 0)) Serial.print("LNMode.Flexible ");
  if (bitRead(s_status, 11)) Serial.print("Lline.AntiTamperL ");
  if (!bitRead(s_status, 11)) Serial.print("Lline.AntiTamperN ");
  if (bitRead(s_status, 12)) Serial.print("RevP.CF1ActiveReverse ");
  if (!bitRead(s_status, 12)) Serial.print("RevP.CF1ActiveForward ");
  if (bitRead(s_status, 13)) Serial.print("RevQ.CF2ReActiveReverse ");
  if (!bitRead(s_status, 13)) Serial.print("RevQ.CF2ReActiveForward ");
  if (bitRead(s_status, 14)) Serial.print("Pnoload.NoLoadActive ");
  if (!bitRead(s_status, 14)) Serial.print("Pnoload.NoLoadNotActive ");
  if (bitRead(s_status, 15)) Serial.print("Qnoload.ReactiveNoLoad ");
  if (!bitRead(s_status, 15)) Serial.print("Qnoload.NotReactiveNoLoad ");
  Serial.println();
  if (s_status == 0x2801) Serial.println(">ATM 0x46- #2801 Accumulator Populated");
  if (s_status == 0xC801) Serial.println(">ATM 0x46- #C801 Accumulator Not Running");
  if (s_status == 0xC800) Serial.println(">ATM 0x46 - #C800 Meter Status Default Value");
  if (s_status == 0xFFFF) Serial.println(">ATM 0x46 - #FFFF Failed | Fault on ATM | Reboot Needed");

    // MMode Metering Status
  yield();
  Serial.print("MMode Status \t\t\t(MMode 0x2B):\t\t0x");
  s_status = eic.GetMModeStatus();
  DisplayHEX(s_status, 4);
  DisplayBIN16(s_status);
  if (!bitRead(s_status, 5) && !bitRead(s_status, 4)) Serial.print("MMode.PositiveZeroCrossing ");
  if (!bitRead(s_status, 5) && bitRead(s_status, 4)) Serial.print("MMode.NegativeZeroCrossing ");
  if (bitRead(s_status, 5) && !bitRead(s_status, 4)) Serial.print("MMode.AllZeroCrossing ");
  if (bitRead(s_status, 5) && bitRead(s_status, 4)) Serial.print("MMode.NoZeroCrossing ");
  if (bitRead(s_status, 10)) Serial.print("MMode.LNSel.LLine(Default) ");
  if (!bitRead(s_status, 10)) Serial.print("MMode.LNSel.NLine ");
  if (!bitRead(s_status, 12) && !bitRead(s_status, 11)) Serial.print("MMode.NLine.CurrentGain2 ");
  if (!bitRead(s_status, 12) && bitRead(s_status, 11)) Serial.print("MMode.NLine.CurrentGain4 ");
  if (bitRead(s_status, 12) && !bitRead(s_status, 11)) Serial.print("MMode.NLine.CurrentGain1 ");
  if (bitRead(s_status, 12) && bitRead(s_status, 11)) Serial.print("MMode.NLine.CurrentGain1 ");
  if (bitRead(s_status, 15)) Serial.print("MMode.CurrentChannelGain1 ");
  if (!bitRead(s_status, 15) && !bitRead(s_status, 14) && !bitRead(s_status, 13)) Serial.print("MMode.LGain.CurrentChannelGain4 ");
  if (!bitRead(s_status, 15) && !bitRead(s_status, 14) && bitRead(s_status, 13)) Serial.print("MMode.LGain.CurrentChannelGain8 ");
  if (!bitRead(s_status, 15) && bitRead(s_status, 14) && !bitRead(s_status, 13)) Serial.print("MMode.LGain.CurrentChannelGain16 ");
  if (!bitRead(s_status, 15) && bitRead(s_status, 14) && bitRead(s_status, 13)) Serial.print("MMode.LGain.CurrentChannelGain24 ");
  Serial.println();
  if (s_status == 0x9422) Serial.println(">ATM 0x2B - #9422 MMode Default Value");

  // ATM Read Values

  Serial.println("-----------");

  // CalStart Status
  yield();
  Serial.print("Calibraration Status \t\t(CalStart 0x20):\t0x");
  s_status = eic.GetCalStartStatus();
  DisplayHEX(s_status, 4);
  DisplayBIN16(s_status);
  if (s_status == 0x6886) Serial.print("Power-On Value. Metering Function is Disabled");
  if (s_status == 0x5678) Serial.print("CALIBRATION | Meter Calibration Startup Command");
  if (s_status == 0x8765) Serial.print("RUNNING | Normal Metering Mode");
  if (s_status != 0x6886 && s_status != 0x5678 && s_status != 0x8765) Serial.print(">ATM 0x20 - Metering Function is Disabled");
  Serial.println();

  yield();
  Serial.print("UGain Calibration Value\t\t(UGain 0x31):\t\t0x");
  s_status = eic.GetUGain();
  DisplayHEX(s_status, 4);
  if (bitRead(s_status, 15)) Serial.print("UGain Possible Value Error");
  Serial.println();

  yield();
  Serial.print("LGain Calibration Value\t\t(LGain 0x23):\t\t0x");
  s_status = eic.GetLGain();
  DisplayHEX(s_status, 4);
  Serial.println();

  yield();
  Serial.print("IGain Calibration Value\t\t(IgainL 0x32):\t\t0x");
  s_status = eic.GetIGain();
  DisplayHEX(s_status, 4);
  Serial.println();

  // Checksum 1 Status
  yield();
  Serial.print("Checksum Status \t\t(CS1 0x2C):\t\t0x");
  s_status = eic.GetCS1Status();
  DisplayHEX(s_status, 4);
  if (s_status != eic.GetCS1Calculated()) { // 0xC000
    Serial.print("*ERROR: Please update _crc1 to ATM Calculated CRC: 0x");
    Serial.print(eic.GetCS1Calculated(), HEX);
  }
  Serial.println();

  // Checksum 2 Status
  yield();
  Serial.print("Checksum Status \t\t(CS2 0x3B):\t\t0x");
  s_status = eic.GetCS2Status();
  DisplayHEX(s_status, 4);
  if (s_status != eic.GetCS2Calculated()) { // 0x3000
    Serial.print("*ERROR: Please update _crc2 to ATM Calculated CRC: 0x");
    Serial.print(eic.GetCS2Calculated(), HEX);
  }
  Serial.println();

  Serial.println("-----------");

  yield();
  Serial.print("Line Voltage \t\t\t(Urms 0x49):\t\t");
  Serial.println(eic.GetLineVoltage());

  yield();
  Serial.print("Line Current \t\t\t(Irms 0x48):\t\t");
  Serial.println(eic.GetLineCurrent());

  yield();
  Serial.print("Line Frequency \t\t\t(Freq 0x4C):\t\t");
  Serial.println(eic.GetFrequency());

  yield();
  Serial.print("Import Energy \t\t\t(APenergy 0x40):\t");
  Serial.println(eic.GetImportEnergy());

  yield();
  Serial.print("Export Energy \t\t\t(ANenergy 0x41):\t");
  Serial.println(eic.GetExportEnergy());

  yield();
  Serial.print("Active power \t\t\t(Pmean 0x4A):\t\t");
  Serial.println(eic.GetActivePower());

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
  s_status = eic.GetLSBStatus();
  DisplayHEX(s_status, 4);
  Serial.println(s_status);

  // Other GTEM Sensors

  // ESP32 ADC 12-Bit SAR (Successive Approximation Register)
  // Conversion resolution 0 - 4095 (4096)
  // You may need to calibrate as needed.

  Serial.println("-----------");

  // DCV_IN
  yield();
  ADC_Constant = 31.340;  // Adjust as needed for calibration of VDC_IN.
  ADC_Voltage = (analogRead(DCV_IN) * ADC_Constant) / 4095;
  Serial.print("DC Voltage Sensor \t\t(DCV_IN VP):\t\t");
  Serial.print(ADC_Voltage);
  if (ADC_Voltage > 20) Serial.print(" *WARNING: Please Check Input Voltage.  Too High!");
  Serial.println();

  // NTC
  yield();
  int Vo;
  float R1 = 10000;  // Based on 10K
  float logR2, R2, T, Tc, Tf;
  float tCal = 1.16;  // Tweak for Calibration
  float C1 = 1.009249522e-03, C2 = 2.378405444e-04, C3 = 2.019202697e-07;

  Vo = analogRead(NTC_IN);
  R2 = R1 * (4095.0 / (float)Vo - tCal);
  logR2 = log(R2);
  T = (1.0 / (C1 + C2 * logR2 + C3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
  // Tf = (Tc * 9.0) / 3.3 + 32.0;  // Fahrenheit
  Serial.print("PCB Temperature Sensor\t\t(NTC_IN VN):\t\t");
  Serial.print(Tc);
  Serial.println(" ºC");

  Serial.println("\n");
}

// **************** SETUP ****************
void setup() {

  // Stabalise
  delay(250);

  // Initialise UART
  Serial.begin(115200, SERIAL_8N1);  //115200
  while (!Serial)
    ;
  Serial.println("");

  // LEDs
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Green, OUTPUT);
  pinMode(LED_Blue, OUTPUT);

  // LEDs Default Off State
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Green, HIGH);
  digitalWrite(LED_Blue, HIGH);

  /*Initialise ATM90E26 + SPI port */
  eic.InitEnergyIC();

  // Stabalise
  delay(250);

  DisplayRegisters();  // Display Registers Once
}

// **************** LOOP ****************
void loop() {

  // Heatbeat LED
  digitalWrite(LED_Red, LOW);
  delay(50);
  digitalWrite(LED_Red, HIGH);

  // Loop Delay
  delay(LoopDelay * 1000);
}
