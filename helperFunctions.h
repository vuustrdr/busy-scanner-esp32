class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
public:
  std::unordered_set<std::string> deviceAddresses;

  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Check if this device has already been detected
    deviceAddresses.insert(advertisedDevice.getAddress().toString());
  }
};

void setWifiCreds() {
  initial = EEPROM.read(addr_3);
  if (initial == 0) {
    for (int i = 0; i < ssidMaxLength; i++) {
      ssid[i] = EEPROM.read(addr + i);
      if (ssid[i] == '\0') break;
    }
    ssid[ssidMaxLength] = '\0';

    for (int i = 0; i < passMaxLength; i++) {
      pass[i] = EEPROM.read(addr_2 + i);
      if (pass[i] == '\0') break;
    }
    pass[passMaxLength] = '\0';
  }
}

void scanAndSend() {
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
  http.end();

  cb.deviceAddresses.clear();
}

void validateCredentials() {

  String response = http.getString();
  StaticJsonDocument<200> jsonDoc;
  deserializeJson(jsonDoc, response);
  const char* serverSsid = jsonDoc["ssid"];
  const char* serverPass = jsonDoc["pass"];

  char domainServerSsid[ssidMaxLength + 1];
  char domainServerPass[passMaxLength + 1];

  strcpy(domainServerSsid, serverSsid);
  strcpy(domainServerPass, serverPass);

  if (domainServerSsid != ssid) {

    for (int i = 0; i < ssidMaxLength; i++) {
      EEPROM.write(addr + i, domainServerSsid[i]);
    }

    if (initial == 1) {
      EEPROM.write(addr_3, 0);
    } //write to 0

    EEPROM.commit();
    esp_restart();
    resetCredentials(addr, domainServerSsid, ssidMaxLength + 1);

  } 
  
  if (domainServerPass != pass) {

    for (int i = 0; i < passMaxLength; i++) {
      EEPROM.write(addr_2 + i, domainServerPass[i]);
    }   

    if (initial == 1) {
      EEPROM.write(addr_3, 0);
    } //write to 0

    EEPROM.commit();  
    esp_restart();

    resetCredentials(addr_2, domainServerPass, passMaxLength + 1);

  } 
}

void resetCredentials(int address, char creds[], int credMaxLength) {

  for (int i = 0; i < credMaxLength; i++) {
    EEPROM.write(address + i, creds[i]);
  }

  if (initial == 1) {
    EEPROM.write(addr_3, 0);
  } 

  EEPROM.commit();
  esp_restart();

}

