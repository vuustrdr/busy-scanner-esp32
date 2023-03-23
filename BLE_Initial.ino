#include <BLEDevice.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <unordered_set>
#include "scannerVars.h"
#include "helperFunctions.h"

// Scan every 10 seconds and send a post request to the configured endpoint.

void setup() {
  Serial.begin(115200);

  Serial.println("Initializing...");

  BLEDevice::init("");
  // BLEDevice::setPower(ESP_PWR_LVL_P9);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting");
    delay(1000);
  }

  Serial.println("Connected");
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    scanAndSend();

  }

  delay(10000);
}

