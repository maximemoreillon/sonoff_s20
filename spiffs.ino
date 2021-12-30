void spiffs_setup() {

  Serial.println("[LittleFS] Retrieving config from LittleFS");
  
  if (!LittleFS.begin()) {
    Serial.println("[LittleFS] Failed to mount file system");
  }
  else {
    Serial.println("[LittleFS] File system mounted");
  }
}

void get_config_from_spiffs(){

  StaticJsonDocument<1024> doc;

  
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("[LittleFS] Failed to open config file");
    return;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("[LittleFS] Config file size is too large");
    return;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("[LittleFS] Failed to parse config file");
    return;
  }

  // export file content to custom config structure
  config.nickname = doc["nickname"].as<String>();
  config.wifi.ssid = doc["wifi"]["ssid"].as<String>();
  config.wifi.password = doc["wifi"]["password"].as<String>();
  config.mqtt.broker.host = doc["mqtt"]["broker"]["host"].as<String>();
  config.mqtt.broker.port = doc["mqtt"]["broker"]["port"].as<int>();
  config.mqtt.username = doc["mqtt"]["username"].as<String>();
  config.mqtt.password = doc["mqtt"]["password"].as<String>();

}
