class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
public:
  std::unordered_set<std::string> deviceAddresses;

  void onResult(BLEAdvertisedDevice advertisedDevice) {
    deviceAddresses.insert(advertisedDevice.getAddress().toString());
  }
};

void scanAndSend() {

  BLEScan* pBLEScan = BLEDevice::getScan();
  MyAdvertisedDeviceCallbacks cb;
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5);

  int deviceCount = cb.deviceAddresses.size();
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");
  int response = http.POST("{\"devices\": " + String(deviceCount) + "}");
  http.end();

  cb.deviceAddresses.clear();

}
