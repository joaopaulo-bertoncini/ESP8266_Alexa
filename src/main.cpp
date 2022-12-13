#include <Arduino.h>
#include "network.h"

#define RELAY_PIN_1 5
#define LAMP "Lâmpada 1"

void setup() {
  Serial.begin(9600);
  scanNetworks();
  printWiFiData();
}

void loop() {

}