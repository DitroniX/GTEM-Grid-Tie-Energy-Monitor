**GTEM ESP32 ATM90E26 Energy Monitoring Energy Monitor**

------------

**ESP32 Interfacing**

The ESP 32 interfaces to the ATM90E26 via either:
- SPI
- UART (connected to ESP32 UART2)

*This is user selectable using the pads on the bottom of the GTEM board.  By default, SPI is selected.*

The ESP32 interfaces to the AT24C64 EEPROM via I2C.  It has an address of 0x50

Flashing and Debugging connection to the ESP32, is via the USB UART (CH340).

**Pads**

The GTEM board has a number of solder pads on the bottom.

Default settings:
- **SPI** ATM Interface **Enabled**
- **Metering Mode L+N** 
- **CT** Clamp **Burden** Resistor **Enabled**
- **OLED** **Pin 1 Ground - Pin 2 3V3** 
- **SPI CS Open**
- **OPTO** Pull up **Disabled** Open
- **User Jumper GP25** Disabled **Open** 


The defaults are shown with a line above the pads to short.

![Display-Type-B](https://ditronix.net/wp-content/uploads/2022/12/GTEM-ESP32-ATM90E26-SDK-v1-2212-104-PCB-Bottom.png?raw=true)


**ESP32 GPIO**

- **21**	**SDA**	I2C.  Pulled High via 10K
- **22**	**SCL**	I2C.  Pulled High via 10K

------------


- **12**		**User GP12 GPIO**	Strapping Pin. Boot fail if pulled HIGH
- **13**		ATM_**IRQ**	
- **14**		**User GP14 GPIO**	Note: ESP outputs PWM at boot
- **15**		**LED BLUE**	Strapping Pin. Outputs PWM at boot

------------


- **19**	**MISO**		V-SPI
- **23**	**MOSI**		V-SPI
- **18**	**SCK**	V-SPI
- **5**		**CS**	V-SPI.  Strapping Pin. Outputs PWM at boot

------------
- **1**	TX0	**USB_TXD**	Debug output at boot
- **3**	RX0	**USB_RXD**	HIGH at boot

------------


- **17**	TX2	**TTL_TXD**	
- **16**	RX2	**TTL_RXD**	

------------


- **0**	**GP0**	Strapping Pin. Outputs PWM at boot
- **2**	LED	**LED RED**	Strapping Pin. Connected to on-board ESP LED
- **4**	ADC 2 CH0	**LED GREEN**	Strapping Pin. Ignore 0 - 0.1V and 3.2 - 3.3V

------------


- **27**	ADC 2 CH7	**ATM_WO**	Ignore 0 - 0.1V and 3.2 - 3.3V
- **32**	ADC 1 CH4	**ATM_ZX**	Ignore 0 - 0.1V and 3.2 - 3.3V
- **33**	ADC 1 CH5	**OPTO**	Ignore 0 - 0.1V and 3.2 - 3.3V
- **34**	ADC 1 CH6	**ATM_CF1**	input only. Ignore 0 - 0.1V and 3.2 - 3.3V
- **35**	ADC 1 CH7	**ATM_CF2**	input only. Ignore 0 - 0.1V and 3.2 - 3.3V

------------


- **36**	VP / ADC 1 CH0	PCA DC Supply **DCV_IN**	input only
- **39**	VN / ADC 1 CH3	**NTC**-10K	input only

------------


- **25**	DAC1	**User_Jumper**	With pullup and decoupling cap
- **26**	DAC2	**User_Button**	With pullup and decoupling cap


------------

The below GPIO must be in the following states when ESP32 boots, or it will fail to boot.

- GP0	High
- GP3	High
- GP5	High
- GP12	Low
- GP15	High



Further information can be found:

- **Website** https://ditronix.net/
- **WIKI**  https://ditronix.net/wiki/
- **GitHub**  https://github.com/DitroniX/
- **Shop**  https://ditronix.net/shop/
- **Hackster.io** https://www.hackster.io/ditronix

Dave Williams | G8PUO | DitroniX 1981-2024 | ditronix.net
