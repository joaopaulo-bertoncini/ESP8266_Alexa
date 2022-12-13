#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <SPI.h>
// DEFINE HERE THE KNOWN NETWORKS
const char* KNOWN_SSID[] = {"", ""};
const char* KNOWN_PASSWORD[] = {"", ""};
const int   KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); // number of known networks
WiFiClient client;
String host = "192.168.0.101";
const uint16_t port = 5000;
IPAddress robotIP, subnetMask, gatewayIP;

void scanNetworks() {
  boolean wifiFound = false;
  int i, n;
  // ----------------------------------------------------------------
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  // ----------------------------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println(">> SETUP DONE");
  // ----------------------------------------------------------------
  // WiFi.scanNetworks will return the number of networks found
  // ----------------------------------------------------------------
  Serial.println(F(">> SCAN START"));
  int nbVisibleNetworks = WiFi.scanNetworks();
  Serial.println(F(">> SCAN DONE"));
  if (nbVisibleNetworks == 0) {
    Serial.println(F(">> NO NETWORKS FOUND. RESET TO TRY AGAIN"));
    while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // ----------------------------------------------------------------
  // if you arrive here at least some networks are visible
  // ----------------------------------------------------------------
  Serial.print(nbVisibleNetworks);
  Serial.println(">> NETWORK(S) FOUND");

  // ----------------------------------------------------------------
  // check if we recognize one by comparing the visible networks
  // one by one with our list of known networks
  // ----------------------------------------------------------------
  for (i = 0; i < nbVisibleNetworks; ++i) {
    Serial.println(WiFi.SSID(i)); // Print current SSID
    for (n = 0; n < KNOWN_SSID_COUNT; n++) { // walk through the list of known SSID and check for a match
      if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str())) {
        Serial.print(F("\tNot matching "));
        Serial.println(KNOWN_SSID[n]);
      } else { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound) break; // break from the "for each visible network" loop
  } // end for each visible network

  if (!wifiFound) {
    Serial.println(F(">> NO KNOWN NETWORK IDENTIFIED. RESET TO TRY AGAIN"));
    while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // ----------------------------------------------------------------
  // if you arrive here you found 1 known SSID
  // ----------------------------------------------------------------
  Serial.print(F("\n>> CONNECTING TO "));
  Serial.println(KNOWN_SSID[n]);

  // ----------------------------------------------------------------
  // We try to connect to the WiFi network we found
  // ----------------------------------------------------------------
  WiFi.begin(KNOWN_SSID[n], KNOWN_PASSWORD[n]);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(""); 
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void printWiFiData() {
  // ----------------------------------------------------------------
  // SUCCESS, you are connected to the known WiFi network
  // ----------------------------------------------------------------
  // print your WiFi 101 Shield's IP address:
  robotIP = WiFi.localIP();
  Serial.print(">> IP address: ");
  Serial.println(robotIP);

  Serial.print(">> Subnet mask: ");
  subnetMask = WiFi.subnetMask();
  Serial.println(subnetMask);

  Serial.print(">> Gateway IP: ");
  gatewayIP = WiFi.gatewayIP();
  Serial.println(gatewayIP);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print(">> MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print(">> SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print(">> signal strength (RSSI): ");
  Serial.println(rssi);
}

bool pingNetwork() {
  IPAddress remote_ip = robotIP;
  for (size_t i = 1; i < subnetMask[2]; i++)
  {   
    Serial.print("Pinging host ");
    remote_ip[3] = i;
    Serial.println(remote_ip);
    if(Ping.ping(remote_ip)) {
      Serial.println("Success!!");
    } else {
      Serial.println("Error :(");
    }
  }
  return true;
}

bool scanServe() {
  IPAddress remote_ip = robotIP;
  Serial.println(">>CONECTING IN " + host);
  if (client.connect(host, port)) {
    Serial.print(">>connected ");
  } else {
    Serial.print(">>SCAN SERVE ");
    for (size_t i = 1; i < 256; i++)
    {   
      host = String(remote_ip[0]) + "." + String(remote_ip[1]) + "." + String(remote_ip[2]) + "." + String(i);
      Serial.println(">>CONECTING IN " + host);
      if (client.connect(host, port)) {
        Serial.print(">>connected ");
        break;
      }  
    }
  }
  if (client.connected()) {
    Serial.println(">>SENDING ROBOT NAME TO SERVER "); 
    client.println("<ROBOT-01/>");
  }  
  return true;
}

void connected() { 
  Serial.print(">>CONNECTING TO ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, port)) {
    Serial.println(">>CONNECTION FAILED");
    delay(5000);
    return;
  }  
  // This will send a string to the server
  Serial.println(">>SENDING ROBOT NAME TO SERVER ");
  if (client.connected()) {
    client.println("<ROBOT-01/>");
  }  
}