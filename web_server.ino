class CaptiveRequestHandler : public AsyncWebHandler {
  // Captive portal 
  
  public:
  
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}
  
    bool canHandle(AsyncWebServerRequest *request){
      //request->addInterestingHeader("ANY");
      return true;
    }
  
    void handleRequest(AsyncWebServerRequest *request) {
      request->send(LittleFS, "/index.html", String(), false, processor);
    }
};

String processor(const String& var){

  
  if(var == "DEVICE_NAME") return get_device_name();
  else if(var == "DEVICE_TYPE") return DEVICE_TYPE;
  else if(var == "DEVICE_FIRMWARE_VERSION") return DEVICE_FIRMWARE_VERSION;
  else if(var == "DEVICE_NICKNAME") return config.nickname;
  else if(var == "DEVICE_STATE") return get_device_state();
  
  else if(var == "MQTT_BROKER_HOST") return config.mqtt.broker.host;
  else if(var == "MQTT_BROKER_PORT") return String(config.mqtt.broker.port);
  else if(var == "MQTT_USERNAME") return config.mqtt.username;
  else if(var == "MQTT_PASSWORD") return config.mqtt.password;
  else if(var == "MQTT_STATUS") return MQTT_client.connected() ? "connected" : "disconnected";
  
  else if(var == "WIFI_MODE") return String(WiFi.getMode());
  else if(var == "WIFI_SSID") return config.wifi.ssid;
  else if(var == "WIFI_SSID") return config.wifi.password;

}

String reboot_html = ""
  "Rebooting..."
  "<script>setTimeout(() => window.location.replace('/'), 5000)</script>";


void web_server_setup(){
  Serial.println("[Web server] Web server initialization");
  
  web_server.serveStatic("/", LittleFS, "/www")
    .setDefaultFile("index.html")
    .setTemplateProcessor(processor);
  
  web_server.on("/settings", HTTP_POST, update_settings);
  
  web_server.on("/update", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                  size_t len, bool final) {handleDoUpdate(request, filename, index, data, len, final);}
  );

  web_server.onNotFound(handle_not_found);
  web_server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  web_server.begin();
}


void handle_not_found(AsyncWebServerRequest *request){
  request->send(404, "text/html", "Not found");
}

void save_config(AsyncWebServerRequest *request){
  
  StaticJsonDocument<1024> doc;
  
  doc["nickname"] = request->arg("nickname");
  
  JsonObject wifi  = doc.createNestedObject("wifi");
  wifi["ssid"] = request->arg("wifi_ssid");
  wifi["password"] = request->arg("wifi_password");

  JsonObject mqtt  = doc.createNestedObject("mqtt");
  mqtt["username"] = request->arg("mqtt_username");
  mqtt["password"] = request->arg("mqtt_password");

  JsonObject broker  = mqtt.createNestedObject("broker");
  broker["host"] = request->arg("mqtt_broker_host");
  broker["port"] = request->arg("mqtt_broker_port");

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("[LittleFS] Failed to open config file for writing");
    return;
  }

  serializeJson(doc, configFile);
  Serial.println("[LittleFS] Finished writing config file");
}

void update_settings(AsyncWebServerRequest *request) {

  // TODO: Check if all arguments are set

  save_config(request);

  request->send(200, "text/html", reboot_html);

  // Reboot
  delayed_reboot();
   
}

void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  
  if (!index){
    
    Serial.print("[Update] Updating firmware using file ");
    Serial.println(filename);
    Serial.print("[Update] Size:  ");
    Serial.print(request->contentLength());
    Serial.print(", available: ");
    Serial.print(ESP.getFreeSketchSpace());
    Serial.println();
    
    Update.runAsync(true);
    if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
      Update.printError(Serial);
    }
  }

  if(!Update.hasError()){
    if(Update.write(data, len) != len){
      Update.printError(Serial);
    }
  }


  if (final) {
    if (!Update.end(true)){
      Update.printError(Serial);
      request->send(500, "text/html", "Firmware update failed");
    } 
    else {
      Serial.println("Update complete");
      request->send(200, "text/html", reboot_html);
      delayed_reboot();
    }
  }
}
