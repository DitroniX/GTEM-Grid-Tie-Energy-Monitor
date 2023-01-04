**GTEM ESP32 ATM90E26 Energy Monitor SDK**

GTEM-1_Test_ATM90E26_Basic_Calibration-Domoticz (Visual Studio Platform IO)

This code will

	1) Configure the GTEM board and Test the ATM90E26 Energy Monitor.  
	2) Calibrate the ATM90E26, where needed (Based on Energy Setpoint Calculator GTEM Bring-Up Only.xlsx)
	3) Provide a Register Status and Diagnostic Report (See _Example Report.txt)
	4) Update the values to Domoticz (only when EnableDomoticz flag set)
	
**Board Hardware Test:**	

- Download Code
- Open folder in Visual Studio Code | Platform IO
- Connect the GTEM board USB.  The **platformio.ini** settings should auto select the right board ('Wemos D1 MINI ESP32'and auto detect the right comm port. 
   - Compile / Upload
   - Open the Serial Monitor
- Press 'Reset' on the GTEM Board.  Upon boot, you should see the Diagnostic Page appear in the monitor and values shown.  Ignore the Press board reset to refresh.
   - Example Line Voltage (Urms 0x49): 0.01 V
   - Example Line Current (Irms 0x48): 0.01 A
   - Example Line Frequency (Freq 0x4C): 102.31 Hz
- Connect 12V AC to the GTEM board AC/DC In terminals. (It does not matter which way round the wires go).
- Press 'Reset' on the GTEM Board.  Upon boot, you should see the Line Voltage increase to around your mains voltage.
   - Example Line Voltage (Urms 0x49): 247.47 V
   - Example Line Current (Irms 0x48): 0.01 A
   - Example Line Frequency (Freq 0x4C): 49.99 Hz   
- Connect a CT Clamp onto the Live Tail or test mains cable.  Upon boot, you should see the Line Current update to reflect approximate current
   - Example Line Voltage (Urms 0x49): 247.47 V
   - Example Line Current (Irms 0x48): 1.63A
   - Example Line Frequency (Freq 0x4C): 49.99 Hz

**Calibrate**

Now the above is proven to work, you may wish to calibrate further. To do this open the Excel spreadsheet and update the values in pink (Input Cell), as needed.
- Note, changes to any values, which is then recalculated in Excel, will result in a change of the register hex value in Yellow.  Example Ugain.

The below area of code is the main defaults which could be changed:

      // Calibration Defaults
      ATM90E26_SPI::ATM90E26_SPI(int pin) 
	{
	  _cs = pin;
	  _lgain = 0x1D39; // Use XLS to calculate these values. Examples: 0x1D39;
	  _ugain = 0xA07E; // Use XLS to calculate these values. Examples: 0xA07E; 0x7AF3; 0xD464;  0xA028;
	  _igain = 0x2F6E; // Use XLS to calculate these values. Examples: 0x2F6E; 0x6E49;  0x7DFB; 0x7A13;
	  _crc1 = 0xAE70;  // Important! Run this application, then take auto calculated values and update here
	  _crc2 = 0xDA60;  // Important! Run this application, then take auto calculated values and update here
	}

**Update Registers**

Should you wish to update any register values, you may do so in file **GTEM-1_Defaults.h**.  // Calibration Defaults or // Register Defaults
- Note, register values changes will require an update of the CRC1 or CRC2.  
   - By updating the firmware and rebooting, you may be prompted to update CRC1 or CRC2 with new values  
   - Example: Checksum Status (CS1 0x2C): 0xAE71 *ERROR: Please update _crc1 to ATM Calculated CRC: 0xAE70
- Take note of the CRC1 or CRC2 values (copy) and update (paste) the defaults values as needed.  Example of defaults.
   - // Calibration Defaults
   - _crc1 = 0xAE70;
   - _crc2 = 0xDA60;
- Rebuild the code, upload and upon reboot, you should NOT see any CRC errors displayed.


**Important, you MUST update/correct the CRC otherwise the register values returned maybe erroneous.**


**Enabling Domoticz**

Now you are at a stage to enable publishing to Domoticz. 

Stages are:
- **Domoticz**
   - Domoticz > Setup > Hardware - Create three virtual sensors (name these as you wish)
  	 - LineVoltage ~ Sensor Type = Voltage
  	 - LineCurrent ~ Sensor Type = Ampere (1 Phase)
  	 - ActivePower ~ Sensor Type = Useage 
- **Domoticz.h**
   - WiFi - enter ssid and password
   - Domoticz - enter domoticz_server IP
   - Domoticz Devices Indexes - Update    
  	 - LineVoltage = Index (found in Domoticz > Setup > Devices)
  	 - LineCurrent = Index (found in Domoticz > Setup > Devices)
  	 - ActivePower = Index (found in Domoticz > Setup > Devices)   	 
- **main.cpp**
   - // Constants > EnableDomoticz = true;
   - Rebuild the code, upload and upon reboot you should start to publish
   - The code will now loop and fresh Domoticz, based on the LoopDelay value (Default 1 Second)




A video and further information on calibration to follow.

Further information can be found:

- **Website** https://ditronix.net/
- **WIKI**  https://ditronix.net/wiki/
- **GitHub**  https://github.com/DitroniX/
- **Shop**  https://ditronix.net/shop/

Dave Williams | G8PUO | DitroniX 1981-2023 | ditronix.net
