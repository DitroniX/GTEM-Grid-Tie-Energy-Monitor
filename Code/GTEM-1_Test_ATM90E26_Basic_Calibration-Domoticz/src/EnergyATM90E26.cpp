/*
  Dave Williams, DitroniX 2019-2023 (ditronix.net)
  GTEM-1 ATM90E26 Energy Monitoring Energy Monitor  v1.0
  Features include ESP32 GTEM ATM90E26 16bit ADC EEPROM OPTO CT-Clamp Current Voltage Frequency Power Factor GPIO I2C OLED SMPS D1 USB
  PCA 1.2212-104 - Test Code Firmware v1

  The purpose of this test code is to cycle through the various main functions of the board, as shown below, as part of board bring up testing.

  Simplified Board Bring Up Test -GTEM ATM90E26 Energy Monitor ASIC - Basic Calibration Limits.  Requires Calibration.
  Additional diagnostic serial reporting has been included, for reference and expanded detail.

  Instructions.  See GitHub.com/DitroniX or DitroniX.net/Wiki for further information.

  Code register formulation based on the excellent ground work from Tisham Dhar, whatnick | ATM90E26 Energy Monitor | Code upgraded and updated by Date Williams

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
#include <EnergyATM90E26.h>

// Set
void ATM90E26_SPI::SetLGain(unsigned short lgain)
{
  _lgain = lgain;
}
void ATM90E26_SPI::SetUGain(unsigned short ugain)
{
  _ugain = ugain;
}
void ATM90E26_SPI::SetIGain(unsigned short igain)
{
  _igain = igain;
}
void ATM90E26_SPI::SetCRC1(unsigned short crc1)
{
  _crc1 = crc1;
}
void ATM90E26_SPI::SetCRC2(unsigned short crc2)
{
  _crc2 = crc2;
}

// **************** FUNCTIONS / ROUTINES / CLASSES ****************

// Read
unsigned short ATM90E26_SPI::CommEnergyIC(unsigned char RW, unsigned char address, unsigned short val)
{

  unsigned char *data = (unsigned char *)&val;
  unsigned short output;

  // SPI interface rate is 200 to 160k bps. It Will need to be slowed down for EnergyIC
  SPISettings settings(200000, MSBFIRST, SPI_MODE3);

  // switch MSB and LSB of value
  output = (val >> 8) | (val << 8);
  val = output;

  // Set read write flag
  address |= RW << 7;

  // Transmit and receive data
  SPI.beginTransaction(settings);

  digitalWrite(_cs, LOW);
  delayMicroseconds(10);
  SPI.transfer(address);
  /* Must wait 4 us for data to become valid */
  delayMicroseconds(4);

  // Read data
  // Do for each byte in transfer
  if (RW)
  {
    for (byte i = 0; i < 2; i++)
    {
      *data = SPI.transfer(0x00);
      data++;
    }
  }
  else
  {
    for (byte i = 0; i < 2; i++)
    {
      SPI.transfer(*data); // write all the bytes
      data++;
    }
  }

  digitalWrite(_cs, HIGH);
  delayMicroseconds(10);

  SPI.endTransaction();

  output = (val >> 8) | (val << 8); // reverse MSB and LSB
  return output;
  // Use with transfer16
  // return val;
}

unsigned short ATM90E26_SPI::GetMeterStatus()
{
  return CommEnergyIC(1, EnStatus, 0xFFFF);
}

unsigned short ATM90E26_SPI::GetCalStartStatus()
{
  return CommEnergyIC(1, CalStart, 0xFFFF);
}

unsigned short ATM90E26_SPI::GetUGain()
{
  return CommEnergyIC(1, Ugain, 0xFFFF);
}

unsigned short ATM90E26_SPI::GetLGain()
{
  return CommEnergyIC(1, Lgain, 0xFFFF);
}

unsigned short ATM90E26_SPI::GetIGain()
{
  return CommEnergyIC(1, IgainL, 0xFFFF);
}

unsigned short ATM90E26_SPI::GetLSBStatus()
{
  return CommEnergyIC(1, LSB, 0xFFFF);
}

unsigned short ATM90E26_SPI::GetMModeStatus()
{
  return CommEnergyIC(1, MMode, 0xFFFF);
}

unsigned short ATM90E26_SPI::GetCS1Status()
{
  return _crc1;
}

unsigned short ATM90E26_SPI::GetCS2Status()
{
  return _crc2;
}

unsigned short ATM90E26_SPI::GetCS1Calculated()
{
  return CommEnergyIC(1, CSOne, 0x0000);
}

unsigned short ATM90E26_SPI::GetCS2Calculated()
{
  return CommEnergyIC(1, CSTwo, 0x0000);
}

double ATM90E26_SPI::GetLineVoltage()
{
  unsigned short voltage = CommEnergyIC(1, Urms, 0xFFFF);
  return (double)voltage / 100;
}

double ATM90E26_SPI::GetLineCurrent()
{
  unsigned short current = CommEnergyIC(1, Irms, 0xFFFF);
  return (double)current / 1000;
}

double ATM90E26_SPI::GetActivePower()
{
  short int apower = (short int)CommEnergyIC(1, Pmean, 0xFFFF); // Complement, MSB is signed bit
  return (double)apower;
}

double ATM90E26_SPI::GetFrequency()
{
  unsigned short freq = CommEnergyIC(1, Freq, 0xFFFF);
  return (double)freq / 100;
}

double ATM90E26_SPI::GetPowerFactor()
{
  short int pf = (short int)CommEnergyIC(1, PowerF, 0xFFFF); // MSB is signed bit
  // if negative
  if (pf & 0x8000)
  {
    pf = (pf & 0x7FFF) * -1;
  }
  return (double)pf / 1000;
}

double ATM90E26_SPI::GetReactivefwdEnergy()
{
  // Register is cleared after reading
  unsigned short rpower = CommEnergyIC(1, RPenergy, 0xFFFF); // 0xFF, 0xFF, 0x42);
  return (double)rpower / 10 / 1000;                         // returns kWh if PL constant set to 1000imp/kWh
}

double ATM90E26_SPI::GetImportEnergy()
{
  // Register is cleared after reading
  unsigned short ienergy = CommEnergyIC(1, APenergy, 0xFFFF);
  return (double)ienergy * 0.0001; // returns kWh if PL constant set to 1000imp/kWh
}

double ATM90E26_SPI::GetExportEnergy()
{
  // Register is cleared after reading
  unsigned short eenergy = CommEnergyIC(1, ANenergy, 0xFFFF);
  return (double)eenergy * 0.0001; // returns kWh if PL constant set to 1000imp/kWh
}

unsigned short ATM90E26_SPI::GetSysStatus()
{
  return CommEnergyIC(1, SysStatus, 0xFFFF);
}

double ATM90E26_SPI::GetAbsActiveEnergy()
{
  // Register is cleared after reading
  unsigned short absenergy = CommEnergyIC(1, ATenergy, 0xFFFF); // 0xFF, 0xFF, 0x41);
  return (double)absenergy / 10 / 1000;                         // returns kWh if PL constant set to 1000imp/kWh
}

double ATM90E26_SPI::GetAbsReactiveEnergy()
{
  // Register is cleared after reading
  unsigned short abspower = CommEnergyIC(1, Rtenergy, 0xFFFF); // 0xFF, 0xFF, 0x44);
  return (double)abspower / 10 / 1000;                         // returns kWh if PL constant set to 1000imp/kWh
}
