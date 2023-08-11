## GTEM ESP32 ATM90E26 Energy Monitor SDK





GTEM Overview
![Display-Type-B](https://github.com/DitroniX/GTEM-Grid-Tie-Energy-Monitor/blob/main/Datasheets%20and%20Information/GTEM%20Power%20Energy%20Monitor%20Board%20Overview.jpg?raw=true)

GTEM 'What Is In The Shipping Box'
![Display-Type-B](https://ditronix.net/wp-content/uploads/2023/07/GTEM-ESP32-ATM90E26-In-The-Box-scaled.jpg?raw=true)

**DitroniX.net Power Energy Monitor Boards SDK Overview**
![Display-Type-B](https://ditronix.net/wp-content/uploads/2023/08/DitroniX.net-Power-Energy-Monitor-Boards-SDK-Overview-scaled.jpg?raw=true)


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


## **Further Information**

Additional information, and other technical details on this project, maybe found in the related repository pages.

**Repository Folders**

 - **Code** *(Code examples for Arduino  IDE and PlatformIO)*
 -  **Datasheets and Information** *(Component Datasheets, Schematics, Board Layouts, Photos, Technical Documentation)*
 - **Certification** *(Related Repository Project or Part, Certification Information)*

**Repository Tabs**

 - **Wiki** *(Related Repository Wiki pages and Technical User Information)*
 - **Discussions** *(Related Repository User Discussion Forum)*
 - **Issues** *(Related Repository Technical Issues and Fixes)*

***

We value our Customers, Users of our designs and STEM Communities, all over the World . Should you have any other questions, or feedback to share to others, please feel free to:

* Visit the related [Project](https://github.com/DitroniX?tab=repositories) *plus the related* **Discussions** and **Wiki** Pages.  See tab in each separate repository.
* **Project Community Information** can be found at https://www.hackster.io/DitroniX
* [DitroniX.net Website - Contact Us](https://ditronix.net/contact/)
* **Twitter**: [https://twitter.com/DitroniX](https://twitter.com/DitroniX)
* [Supporting the STEM Projects - BuyMeACoffee](https://www.buymeacoffee.com/DitroniX)
*  **LinkedIN**: [https://www.linkedin.com/in/g8puo/](https://www.linkedin.com/in/g8puo/)

***Dave Williams, Eastbourne, UK.***

Electronics Engineer | Software Developer | R&D Support | RF Engineering | Product Certification and Testing | STEM Ambassador

## STEM

**Supporting [STEM Learning](https://www.stem.org.uk/)**

Life is one long exciting learning curve, help others by setting the seed to knowledge.

![DitroniX Supporting STEM](https://hackster.imgix.net/uploads/attachments/1606838/stem_ambassador_-_100_volunteer_badge_edxfxlrfbc1_bjdqharfoe1_xbqi2KUcri.png?auto=compress%2Cformat&w=540&fit=max)81-2023 | ditronix.net
