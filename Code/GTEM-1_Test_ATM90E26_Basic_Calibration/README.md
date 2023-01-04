**GTEM ESP32 ATM90E26 Energy Monitor SDK**

GTEM-1_Test_ATM90E26_Basic_Calibration. The GTEM boards are all tested prior to shipping, based on the below guidance and calibration code.  For this reason, you may only need to tweak mains voltage and current values slightly.

This code will configure the GTEM board, Test the hardware and ATM90E26 Energy Monitor.

- Download Code
- Open the code in Arduino IDE
- With the GTEM board connected via USB only, Select Comm Port and Select Device 'Wemos D1 MINI ESP32'
   - Compile / Upload
   - Open the Serial Monitor
- Press 'Reset' on the GTEM Board.  Upon boot, you sould see the Diagnostic Page appear ONCE in the monitor and values shown.  Press board reset to refresh.
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



Now the above is proven to work in your setup (and maybe acceptable limits), you may wish to calibrate further (as previously mentioned main area being mains voltage and current). To do this open the Excel spreadsheet and update the values in pink, as needed.
- Note, changes to any values, which is then recalculated in Excel, will result in a change of the register hex value in Yellow.  Example Ugain.

The below area of code is the main defaults which could be changed:

      // Calibration Defaults
      ATM90E26_SPI::ATM90E26_SPI(int pin) {
      _cs = pin;
      _lgain = 0x1D39;  // Use XLS to calculate these values. Examples: 0x1D39;
      _ugain = 0xA028;  // Use XLS to calculate these values. Examples: 0xD464;  0xA028;
      _igain = 0x7DFB;  // Use XLS to calculate these values. Examples: 0x6E49;  0x7DFB;
      _crc1 = 0xAE70;   // Run this application. Take auto calculated values and update here
      _crc2 = 0x4BE5;   // Run this application. Take auto calculated values and update here
      }


Should you wish to update any register values, you may do so in file GTEM-1_Defaults.h.  // Calibration Defaults or // Register Defaults
- Note, register values changes will require an update of the CRC1 or CRC2.  
   - By updating the firmware and rebooting, you may be prompted to update CRC1 or CRC2 with new values  
   - Example: Checksum Status (CS1 0x2C): 0xAE71 *ERROR: Please update _crc1 to ATM Calculated CRC: 0xAE70
- Take note of the CRC1 or CRC2 values and update the defaults values as needed.  Example of defaults.
   - // Calibration Defaults
   - _crc1 = 0xAE70;
   - _crc2 = 0x4BE5;
- Rebuild the code, upload and upon reboot, you should NOT see any CRC errors displayed.

A video and further information on calibration to follow.

Further information can be found:

- **Website** https://ditronix.net/
- **WIKI**  https://ditronix.net/wiki/
- **GitHub**  https://github.com/DitroniX/
- **Shop**  https://ditronix.net/shop/

Dave Williams | G8PUO | DitroniX 1981-2023 | ditronix.net
