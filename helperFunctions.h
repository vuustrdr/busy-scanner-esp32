class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
public:
  std::unordered_set<std::string> deviceAddresses;

  void onResult(BLEAdvertisedDevice advertisedDevice) {
    deviceAddresses.insert(advertisedDevice.getAddress().toString());
  }
};

void logger(String log){
  HTTPClient http;
  http.begin(logEndpoint);
  http.addHeader("Authorization", "Bearer " + authKey);
  http.addHeader("Content-Type", "application/json");
  int response = http.POST("{\"log\": \"" + log + "\"}");
  http.end();
}

// Check version, if unequal update
bool updateCheck() {
 
  HTTPClient http;
  http.begin(versionEndpoint);
  http.addHeader("Authorization", "Bearer " + authKey);
  int response = http.GET();

  if (response != 200) return false;    

  DynamicJsonDocument json(1024);
  deserializeJson(json, http.getString());
  int serverVersion = json["version"];
  if (version == serverVersion) {
    return true;
  }

  http.end();

  logger("New update found, updating device.");

  return false;

}

// Scan devices and send to endpoint
void scanAndSend() {

  BLEScan* pBLEScan = BLEDevice::getScan();
  MyAdvertisedDeviceCallbacks cb;
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true);  
  //pBLEScan->start(120); //configure to bring down sensitivity
  pBLEScan->start(60);
  int deviceCount = cb.deviceAddresses.size();

  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Authorization", "Bearer " + authKey);
  http.addHeader("Content-Type", "application/json");
  int response = http.POST("{\"devices\": " + String(deviceCount) + "}");
  http.end();

  cb.deviceAddresses.clear();

}


int
    totalLength,
    time_out,
    currentLength = 0;

void updateFirmware(uint8_t *data, size_t len)
{
    Update.write(data, len);
    currentLength += len;
    Serial.println(String(currentLength) + "/" + String(totalLength));
    if (currentLength != totalLength)
        return;

    Update.end(true);
    
    logger(String(currentLength) + "/" + String(totalLength) + " - Update complete");
    delay(3000);

    ESP.restart();
}

int updateFromServer(String path, String key, int buffer, int time_out) {
  HTTPClient wifi_client;

  if (WiFi.status() != WL_CONNECTED)
  {
      Serial.println("WiFi not connected");
      return 1;
  }
  
  logger("Conecting to path: " + path);

  wifi_client.begin(path);
  wifi_client.addHeader("Authorization", "Bearer " + key);
  int response = wifi_client.GET();
  
  logger("Response: " + String(response));

  if (response != 200)
  {
      
      logger("Check host destination and internet conection.");
      wifi_client.end();
      return 2;
  }

  totalLength = wifi_client.getSize();
  int len = totalLength;
  Update.begin(UPDATE_SIZE_UNKNOWN);
  
  logger("Update size: " + String(totalLength));
  uint8_t buff[buffer] = {0};
  WiFiClient *stream = wifi_client.getStreamPtr();
  
  logger("Starting Update");
  unsigned long delay;
  delay = millis();
  time_out *= 1000;
  while (wifi_client.connected() && (len > 0 || len == -1))
  {
      unsigned long currentMillis = millis();
      if ((unsigned long)(currentMillis - delay) >= time_out)
      {
          
          logger("Update timeout");
          Update.end(false);
          wifi_client.end();
          delay = currentMillis;
          return 3;
      }
      size_t size = stream->available();
      if (size)
      {
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          updateFirmware(buff, c);
          if (len > 0)
              len -= c;
      }
  }
  return 0;
}


