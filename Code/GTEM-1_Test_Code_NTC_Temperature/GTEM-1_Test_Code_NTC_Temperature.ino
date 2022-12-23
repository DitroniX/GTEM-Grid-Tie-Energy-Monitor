/*
  Dave Williams, DitroniX 2019-2022 (ditronix.net)
  GTEM-1 ATM90E26 Energy Monitoring Energy Monitor  v1.0
  Features include ESP32 GTEM ATM90E26 16bit ADC EEPROM OPTO CT-Clamp Current Voltage Frequency Power Factor GPIO I2C OLED SMPS D1 USB
  PCA 1.2212-104 - Test Code Firmware v1 - 20th December 2022

  The purpose of this test code is to cycle through the various main functions of the board, as shown below, as part of board bring up testing.
  Simplified Board Bring Up Test - NTC Temperature (Output sent to Serial Print)

  Remember!
  Set the BOARD to ESP32, 'WEMOS D1 MINI ESP32' DEV Module (or similar).
  You may set the BAUD rate to 921600 to speed up flashing.
  The SDK does NOT need external power to flash.  It will take Power from the USB 5V.

  This test code is OPEN SOURCE and although is is not intended for real world use, it may be freely used, or modified as needed.
  It is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

  Further information, details and examples can be found on our website wiki pages ditronix.net/wiki and github.com/DitroniX
*/

// Libraries
#include <driver/adc.h>

// ****************  VARIABLES / DEFINES / STATIC ****************

// Constants
const int LoopDelay = 1;  // Loop Delay in Seconds

// **************** OUTPUTS ****************
#define LED_Red 2    // Red LED
#define LED_Green 4  // Green LED

// **************** INPUTS ****************
#define NTC_IN 39  //GPIO 39/VN (Analog ADC 1 CH3)

void setup() {

  // Stabalise
  delay(250);

  // Initialise UART:
  Serial.begin(115200, SERIAL_8N1);  //115200
  while (!Serial)
    ;
  Serial.println("");

  // LEDs
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Green, OUTPUT);

  // LEDs Default Off State
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Green, HIGH);
}

void loop() {

  // ESP32 ADC 12-Bit SAR (Successive Approximation Register)
  // Conversion resolution 0 - 4095 (4096)
  // You may need to calibrate as needed.

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
  Tf = (Tc * 9.0) / 3.3 + 32.0;  // 3V2 used as a reduced ADC

  Serial.print("Temperature\t");
  Serial.print(Tf);
  Serial.print(" �F\t");
  Serial.print(Tc);
  Serial.println(" �C");

  // Heatbeat LED
  digitalWrite(LED_Red, LOW);
  delay(50);
  digitalWrite(LED_Red, HIGH);

  // Loop Delay
  delay(LoopDelay * 1000);
}
