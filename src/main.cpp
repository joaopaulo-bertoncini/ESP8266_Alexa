#include <Arduino.h>
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

// prototypes
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness);

// Change this!!
const char* ssid = "JPSB";
const char* password = "";

boolean wifiConnected = false;

Espalexa espalexa;

EspalexaDevice* device1;

#define RelayPin1 5  //D1

void setup()
{
  Serial.begin(9600);
  pinMode(RelayPin1, OUTPUT);
  // Initialise wifi connection
  wifiConnected = connectWifi();
  if(wifiConnected){
    device1 = new EspalexaDevice("Light1", firstLightChanged); //you can also create the Device objects yourself like here
    espalexa.addDevice(device1); //and then add them
    device1->setValue(255); //this allows you to e.g. update their state value at any time!
    espalexa.begin();
  } else
  {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}
 
void loop()
{
   espalexa.loop();
   delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
    //Control the device
    if (brightness == 255)
      {
        digitalWrite(RelayPin1, HIGH);
        Serial.println("Device1 ON");
      }
    else if (brightness == 0)
    {
      digitalWrite(RelayPin1, LOW);
      Serial.println("Device1 OFF");
    }
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}