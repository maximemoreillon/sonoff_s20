/*
 * CUSTOM SONOFF S20 FIRMWARE
 * Maxime MOREILLON
 * 
 * Board type: ITEAD Sonoff S20
 * SSL support: basic
 */

// Libraries
#include <ESP8266WiFi.h> // Main ESP8266 library
#include <PubSubClient.h> // MQTT
#include <ESPAsyncWebServer.h> // Web server
#include <ESPAsyncTCP.h> // Required for web server
#include <Updater.h> // OTA updates
#include <DNSServer.h> // DNS server to redirect wifi clients to the web server
#include <ArduinoJson.h> // JSON library
#include <ESP8266mDNS.h> // Another DNS, to make the device easily accessible on a network
#include <LittleFS.h> // SPIFFS file system


// Information to identify the device
#define DEVICE_TYPE "socket"
#define DEVICE_FIRMWARE_VERSION "0.3.3"

// Pin mapping
#define RELAY_PIN 12
#define LED_PIN 13
#define BUTTON_PIN 0

// MQTT settings
#define MQTT_RECONNECT_PERIOD 1000
#define MQTT_QOS 1
#define MQTT_RETAIN true

// WIFI settings
#define WIFI_STA_CONNECTION_TIMEOUT 20000
#define WIFI_AP_IP IPAddress(192, 168, 4, 1)

// MISC
#define DNS_PORT 53
#define WEB_SERVER_PORT 80

// Initialize objects
WiFiClientSecure wifi_client; 
PubSubClient MQTT_client(wifi_client);
AsyncWebServer web_server(WEB_SERVER_PORT);
DNSServer dns_server; 

// Global variables
boolean reboot = false;
int found_wifi_count = 0;

// Need a custom structure to hold device config
struct WifiConfig {
  String ssid;
  String password;
};

struct MqttBrokerConfig {
  String host;
  int port;
};

struct MqttConfig {
  MqttBrokerConfig broker;
  String username;
  String password;
};

struct DeviceConfig {
  String nickname;
  WifiConfig wifi;
  MqttConfig mqtt;
};

DeviceConfig config;


void setup()
{
  Serial.begin(115200);
  Serial.println("Smart outlet");

  IO_setup();
  spiffs_setup();
  get_config_from_spiffs();
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
  handle_reboot();
  read_button();
}
