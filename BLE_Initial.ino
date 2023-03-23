#include <BLEDevice.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <unordered_set>
#include <esp_system.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <cstring>
#include "scannerVars.h"
#include "helperFunctions.h"

// Scan every 10 seconds and send a post request to the configured endpoint.
// When the wifi fails to connect (i.e the password has been changed), send a get request asking for the updated creds. 
// If they are different restart the machine with new creds

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM.length());

  Serial.println("Initializing...");

  setWifiCreds();

  BLEDevice::init("");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting");
    delay(1000);
  }

  Serial.println("Connected");
}

void loop() {

  // Send the device count to the server
  if (WiFi.status() == WL_CONNECTED) {

    scanAndSend();

  } else { 
    //if wifi disconnects during loop, assume wifi pass or ssid has changed. Get from server.
    HTTPClient http;
    int response = http.begin("http://192.168.1.102:3001/requests/test_3/connect");

    if (response == HTTP_CODE_OK) {
      
      String response = http.getString();
      StaticJsonDocument<200> jsonDoc;
      deserializeJson(jsonDoc, response);
      const char* serverSsid = jsonDoc["ssid"];
      const char* serverPass = jsonDoc["pass"];

      if (serverSsid != ssid) {

        for (int i = 0; i < ssidMaxLength; i++) {
          EEPROM.write(addr + i, ssid[i]);
        }

        if (initial == 1) {
          EEPROM.write(addr_3, 0);
        } //write to 0

        EEPROM.commit();
        esp_restart();

      } 
      
      if (serverPass != pass) {

        for (int i = 0; i < passMaxLength; i++) {
          EEPROM.write(addr_2 + i, pass[i]);
        }   

        if (initial == 1) {
          EEPROM.write(addr_3, 0);
        } //write to 0

        EEPROM.commit();  
        esp_restart();

      } 

    }
    
  }
  // Wait 10 seconds before scanning again
  delay(10000);
}

