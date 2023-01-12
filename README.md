**GTEM ESP32 ATM90E26 Energy Monitor SDK**

------------
GTEM Beta Release (Showing OLED Display Connected)
![Display-Type-B](https://ditronix.net/wp-content/uploads/2022/11/GTEM-ESP32-ATM90E26-SDK-v1-2211-101-OLED-Test-1536x1112.jpg?raw=true)

GTEM Production Release (Board Under Production Test)
![Display-Type-B](https://ditronix.net/wp-content/uploads/2022/12/GTEM-ESP32-ATM90E26-SDK-v1-2212-104-Under-Test-scaled.jpg?raw=true)

GTEM 'What Is In The Shipping Box'

#Pic to be uploaded


The GTEM Energy Monitoring board main features:

- ESP32 WROOM 32
    -Wireless and Bluetooth
- ATM90E26 (Energy Monitor)
	- Metering features fully in compliance with the requirements of IEC62052-11, IEC62053-21 and IEC62053-23.  
	- Active Energy LED / Input to ESP32, Reactive Energy LED / Input to ESP32
- 24C64 EEPROM
- OLED I2C Connector
- Current Clamp Input
	- Example YHDC SCT-013-000
- AC Low Voltage Input
	- Power safely derived from a SELV / Wall AC/AC Power Supply
	- Examples GreenBrook DAT01A or TLC TC TR7
	- On-board 3V3 DC Power Supply
- Arduino D1 Flashing and Programming Compatibility
- OPTO Input
	- Meter Pulse Opto Reader
- On Board NTC (Temperature)
- Reset Button
- User GPIO
- User Programmable Button
- RGB and Power LEDs
- PCB designed to fit into an BMC enclosure
	- Also allows for the display to be included, or not.
	- Size 65mm x 53m
    
![Display-Type-B](https://ditronix.net/wp-content/uploads/2023/01/GTEM-ESP32-ATM90E26-Enery-Monitor-Connections.jpg?raw=true)    

Remember!
- Our SDKs are WeMos D1 Mini flashing compatible
- Set the BOARD to ESP32, 'WEMOS D1 MINI ESP32' DEV Module (or similar).
- You can also set the BAUD rate to 921600 to speed up flashing.
- The SDK does NOT need external power to flash.  It will take Power from the USB 5V.

***You will need to provide external 12V AC for power up of the Energy Monitor functions.*
*You will need to provide a CT Current Clamp.  Ideally YHDC SCT-013-000***

All test code is OPEN SOURCE and although is is not intended for real world use, it may be freely used, or modified as needed.  It is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See Example Code https://github.com/DitroniX/GTEM-Grid-Tie-Energy-Monitor/tree/main/Code/

Further information can be found:

- **Website** https://ditronix.net/
- **WIKI**  https://ditronix.net/wiki/
- **GitHub**  https://github.com/DitroniX/
- **Shop**  https://ditronix.net/shop/

Dave Williams | G8PUO | DitroniX 1981-2023 | ditronix.net
