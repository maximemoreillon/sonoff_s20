/*
 * CUSTOM SONOFF S20 FIRMWARE
 * Maxime MOREILLON
 * 
 * Board type: Generic ESP8266
 * Flash mode: DOUT
 */

// Libraries
#include <ESP8266WiFi.h> // Main ESP8266 library
#include <PubSubClient.h> // MQTT
#include <WiFiClientSecure.h> // Wifi client, used by MQTT
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h> // EEPROM for saving settings
#include <DNSServer.h> // DNS server to redirect wifi clients to the web server
#include <ArduinoJson.h> // JSON, used for the formatting of messages sent to the server
#include <Updater.h>
#include <ESP8266mDNS.h>

// Information to identify the device
#define DEVICE_TYPE "socket"
#define DEVICE_FIRMWARE_VERSION "0.0.21"

// Pin mapping
#define RELAY_PIN 12
#define LED_PIN 13
#define BUTTON_PIN 0

// MQTT settings
#define MQTT_RECONNECT_PERIOD 1000
#define MQTT_BROKER_ADDRESS "mqtt.iot.maximemoreillon.com"
#define MQTT_PORT 30883
#define MQTT_QOS 1
#define MQTT_RETAIN true

// EEPROM
#define EEPROM_WIFI_SSID_ADDRESS 0
#define EEPROM_WIFI_PASSWORD_ADDRESS 100
#define EEPROM_MQTT_USERNAME_ADDRESS 200
#define EEPROM_MQTT_PASSWORD_ADDRESS 300
#define EEPROM_DEVICE_NICKNAME_ADDRESS 400

// WIFI settings
#define WIFI_AP_IP IPAddress(192, 168, 4, 1)

// Web server settings
#define WEB_SERVER_PORT 80

// MISC
#define DNS_PORT 53

// Initialize objects
WiFiClientSecure wifi_client; 
PubSubClient MQTT_client(wifi_client);
AsyncWebServer web_server(WEB_SERVER_PORT);
DNSServer dns_server; 

// Global variables
String device_state = "off";
String wifi_mode = "STA";

void setup()
{
  // Mandatory initial delay
  delay(10);

  Serial.begin(115200);
  Serial.println("Smart switch");

  IO_setup();
  EEPROM.begin(512);
  wifi_client.setInsecure();
  wifi_setup();
  MQTT_setup();
  dns_server.start(DNS_PORT, "*", WIFI_AP_IP);
  web_server_setup();
  MDNS.begin(get_device_name().c_str());
}


void loop() {
  wifi_connection_manager();
  MQTT_connection_manager();
  MQTT_client.loop();
  dns_server.processNextRequest();
  MDNS.update();
}
