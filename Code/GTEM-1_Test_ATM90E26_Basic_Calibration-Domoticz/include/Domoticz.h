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
#include <WiFi.h>

// ****************  VARIABLES / DEFINES / STATIC / STRUCTURES ****************

// WiFi.  Setup with your Wireless Information
const char *ssid = "xxx";     // network SSID - Case Sensitive
const char *password = "xxx"; // network password - Case Sensitive
WiFiClient client;                        // Initialize the client library
String HostNameHeader = "GTEM-";          // Hostname Prefix

// Domoticz Server info.  Setup with your Domoticz IP and Port
const char *domoticz_server = "0.0.0.0"; // IP Address
int port = 8080;                               // Domoticz port

// Set these values to the Domoticz Devices Indexes (IDX).  If Zero, then entry is ignored. Device needs to be created in Domoticz.
int LineVoltage = 0;   // Urms - Line Voltage RMS
int LineCurrent = 0;   // Irms - Line Current RMS
int ActivePower = 0;   // Pmean - Line Mean Active Power
int LineFrequency = 0;  // Freq - Line Voltage Frequency
int ImportEnergy = 0;   // APenergy - Forward Active Energy
int ExportEnergy = 0;   // ANenergy - Reverse Active Energy
int PowerFactor = 0;    // PowerF - Line Power Factor
int DCVoltage = 0;      // PCB DC Input (Derived from AC)
int PCBTemperature = 0; // PCB NTC

// Set this value to the Domoticz Device Group Index (IDX)
int DomoticzBaseIndex = 0; // Currently Unused.  If Zero, then entry is ignored.  Group device needs to be created in Domoticz. WIP.

// ######### FUNCTIONS #########

// Initialise WiFi
void InitialiseWiFi()
{
    // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Attempting to connect to " + String(ssid));

        // Force Hostname
        String Hostname = HostNameHeader + WiFi.macAddress().substring(WiFi.macAddress().length() - 5, WiFi.macAddress().length());
        Hostname.replace(":", "");
        WiFi.setHostname(Hostname.c_str());

        // Wifi Initialisation
        WiFi.begin(ssid, password);

        // Stabalise for slow Access Points
        delay(2000);

        // Wifi Settings
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.persistent(true);

        // Stabalise for slow Access Points
        delay(1000);

        // Wifi Information
        Serial.println("Connection Details:");
        Serial.println("WiFi SSID \t " + String(ssid)) + "(Wifi Station Mode)";
        Serial.printf("WiFi IP \t %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("WiFi GW \t %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf("WiFi MASK \t %s\n", WiFi.subnetMask().toString().c_str());
        Serial.println("WiFi MAC \t " + WiFi.macAddress());
        Serial.printf("WiFi Hostname \t %s\n", WiFi.getHostname());
        Serial.println("WiFi RSSI \t " + String(WiFi.RSSI()));
        Serial.println("");
    }
}

// Publish to Domoticz - Single Values
void PublishDomoticz(int Sensor_Index, float Sensor_Value)
{

    if (Sensor_Index > 0)
    {
        if (client.connect(domoticz_server, port))
        {
            Serial.print("Sending Message to Domoticz #");
            Serial.print(Sensor_Index);
            Serial.print(" ");
            Serial.println(Sensor_Value);

            client.print("GET /json.htm?type=command&param=udevice&idx=");
            client.print(Sensor_Index);

            client.print("&svalue=");
            client.print(Sensor_Value);

            client.println(" HTTP/1.1");
            client.print("Host: ");
            client.print(domoticz_server);
            client.print(":");

            client.println(port);
            client.println("User-Agent: Arduino-ethernet");
            client.println("Connection: close");
            client.println();

            client.stop();
        }
        else
        {
            Serial.println("WiFi or Domoticz Server Not Connected");

            // Stabalise for slow Access Points
            delay(2000);
            InitialiseWiFi();
        }
    }
}

// It is possible to post a Group of Values to a single Virtual Sensor and graph it (i.e. Voltage, Current, Wattage).
// This will require some coding in Domoticz (possible plugin/sensor type).  Feedback welcomed!.
// Publish to Domoticz EXAMPLE - Batch or Group Values Example to Virtual Sensor.  Update as needed.  Future WIP Option.
void PublishDomoticzATM(int Sensor_Index)
{
    if (Sensor_Index > 0)
    {
        if (client.connect(domoticz_server, port))
        {
            Serial.print("Sending ATM Group Message to Domoticz #");
            Serial.print(Sensor_Index);

            client.print("GET /json.htm?type=command&param=udevice&idx=");
            client.print(Sensor_Index);

            // Potential values to select from and batch post to a single sensor
            client.print("&svalue=");
            client.print(String(LineVoltage));
            client.print(";");
            client.print(String(LineCurrent));
            client.print(";0;");
            client.print(String(LineFrequency));
            client.print(";0;");
            client.print(String(ActivePower));
            client.print(";0;");
            client.print(String(ImportEnergy));
            client.print(";0;");
            client.print(String(ExportEnergy));
            client.print(";0;");
            client.print(String(PowerFactor));
            client.print(";0;");
            client.print(String(DCVoltage));
            client.print(";0;");
            client.print(String(PCBTemperature));
            client.print(";0");

            client.println(" HTTP/1.1");
            client.print("Host: ");
            client.print(domoticz_server);
            client.print(":");

            client.println(port);
            client.println("User-Agent: Arduino-ethernet");
            client.println("Connection: close");
            client.println();

            client.stop();
        }
        else
        {
            Serial.println("Not Connected");
            InitialiseWiFi();
        }
    }
}
