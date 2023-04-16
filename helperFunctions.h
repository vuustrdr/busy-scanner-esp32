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
  http.addHeader("Content-Type", "application/json");
  int response = http.POST("{\"log\": \"" + log + "\"}");
  http.end();
}

// Check version, if unequal update
bool updateCheck() {
 
  HTTPClient http;
  http.begin(versionEndpoint);
  int response = http.GET();

  if (response != 200) return false;    

  DynamicJsonDocument json(1024);
  deserializeJson(json, http.getString());
  int serverVersion = json["version"];
  if (version == serverVersion) {
    logger("Versions are the same");
    return true;
  }

  http.end();

  logger("Versions are not the same, updating device");

  return false;

}

// Scan devices and send to endpoint
void scanAndSend() {

  BLEScan* pBLEScan = BLEDevice::getScan();
  MyAdvertisedDeviceCallbacks cb;
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true);  
  //pBLEScan->start(120); //configure to bring down sensitivity
  pBLEScan->start(10);
  int deviceCount = cb.deviceAddresses.size();

  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");
  int response = http.POST("{\"devices\": " + String(deviceCount) + "}");
  http.end();

  cb.deviceAddresses.clear();

}
