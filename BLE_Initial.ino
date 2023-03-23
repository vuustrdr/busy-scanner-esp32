#include <BLEDevice.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <unordered_set>
#include "scannerVars.h"
#include <esp_system.h>
#include <EEPROM.h>


// Scan every 10 seconds and send a post request to the configured endpoint.
// When the wifi fails to connect (i.e the password has been changed), send a get request asking for the updated creds. 
// If they are different restart the machine with new creds

// Use eeprom to store this data, SSIDs can be up to 32 chars and pass is 63 chars

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
public:
  std::unordered_set<std::string> deviceAddresses;

  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Check if this device has already been detected
    deviceAddresses.insert(advertisedDevice.getAddress().toString());
  }
};

void setup() {
  Serial.begin(115200);

  EEPROM.begin(EEPROM.length());



  for (int i = 0; i < MAX_LENGTH; i++) {
    str[i] = EEPROM.read(ADDR + i);
    if (str[i] == '\0') break; // terminate the loop if we encounter a null character
  }

  Serial.println("Initializing...");
  //Serial.print(test);
  BLEDevice::init("");
  //Connect to WiFi
  WiFi.begin("2539 Hyperoptic 1Gbps Broadband", "a77yeyk6");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting");
    delay(1000);
  }

  Serial.println("Connected");

}

void loop() {

  // Send the device count to the server
  if (WiFi.status() == WL_CONNECTED) {

    BLEScan* pBLEScan = BLEDevice::getScan();
    MyAdvertisedDeviceCallbacks cb;
    pBLEScan->setAdvertisedDeviceCallbacks(&cb);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5);

    int deviceCount = cb.deviceAddresses.size();
    Serial.println(deviceCount);
    HTTPClient http;
    http.begin("http://192.168.1.102:3001/requests/testmachine");
    http.addHeader("Content-Type", "application/json");
    int response = http.POST("{\"devices\": " + String(deviceCount) + "}");
    //Serial.println(response);
    http.end();
    // EEPROM.write(addr, "newssid");
    // EEPROM.commit();
  } else { //are you being dumb? if no and the creds are def wrong, send a get request and restart
  
    int response = http.begin("http://192.168.1.102:3001/requests/test_3/connect");
    for (int i = 0; i < ssidMaxLength; i++) {
      EEPROM.write(ADDR + i, ssid[i]);
    }

  }

  // Clear the set of BLE device addresses
  cb.deviceAddresses.clear();

  // restart machine for new wifi
  // esp_restart();

  // Wait 10 seconds before scanning again
  delay(10000);
}

