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

// Libraries
#include <Arduino.h>
#include <SPI.h>

// ****************  VARIABLES / DEFINES / STATIC / STRUCTURES / CONSTANTS ****************

// Registers
#define SoftReset 0x00 // Software Reset
#define SysStatus 0x01 // System Status
#define FuncEn 0x02    // Function Enable
#define SagTh 0x03     // Voltage Sag Threshold
#define SmallPMod 0x04 // Small-Power Mode
#define LastData 0x06  // Last Read/Write SPI/UART Value
#define LSB 0x08       // RMS/Power 16-bit LSB
#define CalStart 0x20  // Calibration Start Command
#define PLconstH 0x21  // High Word of PL_Constant
#define PLconstL 0x22  // Low Word of PL_Constant
#define Lgain 0x23     // L Line Calibration Gain
#define Lphi 0x24      // L Line Calibration Angle
#define Ngain 0x25     // N Line Calibration Gain
#define Nphi 0x26      // N Line Calibration Angle
#define PStartTh 0x27  // Active Startup Power Threshold
#define PNolTh 0x28    // Active No-Load Power Threshold
#define QStartTh 0x29  // Reactive Startup Power Threshold
#define QNolTh 0x2A    // Reactive No-Load Power Threshold
#define MMode 0x2B     // Metering Mode Configuration
#define CSOne 0x2C     // Checksum 1
#define AdjStart 0x30  // Measurement Calibration Start Command
#define Ugain 0x31     // Voltage RMS Gain
#define IgainL 0x32    // L Line Current RMS Gain
#define IgainN 0x33    // N Line Current RMS Gain
#define Uoffset 0x34   // Voltage Offset
#define IoffsetL 0x35  // L Line Current Offset
#define IoffsetN 0x36  // N Line Current Offse
#define PoffsetL 0x37  // L Line Active Power Offset
#define QoffsetL 0x38  // L Line Reactive Power Offset
#define PoffsetN 0x39  // N Line Active Power Offset
#define QoffsetN 0x3A  // N Line Reactive Power Offset
#define CSTwo 0x3B     // Checksum 2
#define APenergy 0x40  // Forward Active Energy
#define ANenergy 0x41  // Reverse Active Energy
#define ATenergy 0x42  // Absolute Active Energy
#define RPenergy 0x43  // Forward (Inductive) Reactive Energy
#define Rnenerg 0x44   // Reverse (Capacitive) Reactive Energy
#define Rtenergy 0x45  // Absolute Reactive Energy
#define EnStatus 0x46  // Metering Status
#define Irms 0x48      // L Line Current RMS
#define Urms 0x49      // Line Voltage RMS
#define Pmean 0x4A     // L Line Mean Active Power
#define Qmean 0x4B     // L Line Mean Reactive Power
#define Freq 0x4C      // Line Voltage Frequency
#define PowerF 0x4D    // L Line Power Factor
#define Pangle 0x4E    // Phase Angle between Voltage and L Line Current
#define Smean 0x4F     // L Line Mean Apparent Power
#define IrmsTwo 0x68   // N Line Current rms
#define PmeanTwo 0x6A  // N Line Mean Active Power
#define QmeanTwo 0x6B  // N Line Mean Reactive Power
#define PowerFTwo 0x6D // N Line Power Factor
#define PangleTwo 0x6E // Phase Angle between Voltage and N Line Current
#define SmeanTwo 0x6F  // N Line Mean Apparent Power

const int energy_CS = 05; // Use CS pin 5 for GTEM

// **************** FUNCTIONS / ROUTINES / CLASSES ****************
class ATM90E26_SPI
{
public:
  ATM90E26_SPI(int pin = energy_CS);

  double GetLineVoltage();
  double GetLineCurrent();
  double GetActivePower();
  double GetImportPower();
  double GetExportPower();
  double GetFrequency();
  double GetPowerFactor();
  double GetImportEnergy();
  double GetExportEnergy();
  double GetAbsActiveEnergy();
  double GetAbsReactiveEnergy();
  double GetReactivefwdEnergy();

  void SetUGain(unsigned short);
  void SetLGain(unsigned short);
  void SetIGain(unsigned short);
  void SetCRC1(unsigned short);
  void SetCRC2(unsigned short);
  void InitEnergyIC();

  unsigned short GetSysStatus();
  unsigned short GetMeterStatus();
  unsigned short GetCalStartStatus();
  unsigned short GetLSBStatus();
  unsigned short GetMModeStatus();
  unsigned short GetCS1Status();
  unsigned short GetCS2Status();
  unsigned short GetCS1Calculated();
  unsigned short GetCS2Calculated();
  unsigned short GetUGain();
  unsigned short GetLGain();
  unsigned short GetIGain();

private:
  unsigned short CommEnergyIC(unsigned char RW, unsigned char address, unsigned short val);
  int _cs;
  unsigned short _lgain;
  unsigned short _ugain;
  unsigned short _igain;
  unsigned short _crc1;
  unsigned short _crc2;
};
