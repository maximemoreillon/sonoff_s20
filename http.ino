void http_config() {  
  iot_kernel.http.on("/turn_on", HTTP_POST, handle_turn_on);
  iot_kernel.http.on("/turn_off", HTTP_POST, handle_turn_off);
}

void handle_turn_on(AsyncWebServerRequest *request) {
  Serial.println(F("[HTTP] turn_on command received"));
  turn_on();
  request->send(200, "text/html", "OK");
  
}

void handle_turn_off(AsyncWebServerRequest *request) {
  Serial.println(F("[HTTP] turn_off command received"));
  turn_off();
  request->send(200, "text/html", "OK");
  
}
