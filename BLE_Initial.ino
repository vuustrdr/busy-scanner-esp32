#include <WiFiManager.h>
#include <strings_en.h>
#include <wm_consts_en.h>
#include <wm_strings_en.h>

#include <BLEDevice.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <unordered_set>
#include <ArduinoJson.h>
#include <Update.h>
#include <web_update.h>


#include "scannerVars.h"
#include "helperFunctions.h"

// Scan every 10 seconds and send a post request to the configured endpoint.

String HOST = "100.97.59.7:3001";
String Dir = "/files/venues/test_8";

web_update webUpdate(HOST, Dir, 1, 0);

void setup() {
  Serial.begin(115200);

  Serial.println("Initializing...");

  BLEDevice::init("");

  WiFiManager wm;

  //wm.resetSettings();

  bool res;

  res = wm.autoConnect("BusyScanner", "venue123");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting");
    delay(1000);
  }
  
  logger("Connected to wifi");
  logger("Upload complete! Now running version " + String(version));
  
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    if (updateCheck() == false) webUpdate.update_wifi();

    scanAndSend();

  }

  delay(10000);
}

