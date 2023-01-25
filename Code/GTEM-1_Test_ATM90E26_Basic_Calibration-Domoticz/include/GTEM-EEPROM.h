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

// ****************  VARIABLES / DEFINES / STATIC / STRUCTURES / CONSTANTS ****************

// Libraries
#include <serialEEPROM.h> // Reference https://github.com/argandas/serialEEPROM

// EEPROM AT24C64 64K (8192 x 8) 32-byte page writes
serialEEPROM extEEPROM(0x50, 8192, 32); // Address, Size, PageSize

// **************** FUNCTIONS AND ROUTINES ****************

// Read 8bit Value
byte readEEPROM(unsigned int addEEPROM)
{
  uint8_t valEEPROM = 0x00;
  valEEPROM = extEEPROM.read(addEEPROM);
  return valEEPROM;
}

// Write 8bit Value
void WriteEEPROM(unsigned int addEEPROM, uint8_t valEEPROM)
{
  extEEPROM.write(addEEPROM, valEEPROM);
}

// Read 16bit Value
uint16_t readEEPROM16(unsigned int addEEPROM)
{
  uint16_t valEEPROM = 0x0000;
  valEEPROM = (extEEPROM.read(addEEPROM + 1) << 8);
  valEEPROM |= extEEPROM.read(addEEPROM);
  return valEEPROM;
}

// Write 16bit Value
void WriteEEPROM16(unsigned int addEEPROM, uint16_t valEEPROM)
{
  extEEPROM.write(addEEPROM, valEEPROM);
  extEEPROM.write(addEEPROM + 1, valEEPROM >> 8);
}

// Clear EEPROM (Only if unlocked and fully clear, if Validation not correct or corruption)
void ClearEEPROM()
{
  if (extEEPROM.read(1) != 0x99) // Check Locked Status - Future Use
  {
    if (extEEPROM.read(0) != 0x20) // Check Validation Byte - Future Use
    {
      Serial.println("\nPreparing New EEPROM.  Please Wait...");

      for (int i = 0; i < 8192; i++)
      {
        extEEPROM.write(i, 0x00);
      }
    }
    else
    {
      Serial.println("\nClearing EEPROM.  Please Wait...");
      for (int i = 1; i < 8192; i++)
      {
        if (extEEPROM.read(i) != 0x00) // Only Clear Used Bytes
        {
          Serial.print(extEEPROM.read(i), HEX);
          extEEPROM.write(i, 0x00);
        }
      }
    }

    WriteEEPROM(0x00, 0x20); // Write Status Byte

    for (int i = 1; i < 8192; i++)
    {
      if (extEEPROM.read(i) != 0x00) // Only Clear Used Bytes
      {
        Serial.print(extEEPROM.read(i), HEX);
      }
    }

    Serial.println("\nEEPROM Ready\n");
  }
  else
    Serial.println("\nEEPROM Locked");
}

// Initialize EEPROM
void InitializeEEPROM() 
{
  /* Initialize the I2C interface and EEPROM */
  Wire.begin();

  if (extEEPROM.read(0) != 0x20)
    ClearEEPROM();

  Serial.print("\nEEPROM Reading CRC1: ");
  Serial.print(readEEPROM16(0x1C), HEX);
  Serial.print("  CRC2: ");
  Serial.println(readEEPROM16(0x1E), HEX);
}
