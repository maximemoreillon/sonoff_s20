/*
 * CUSTOM SONOFF S20 FIRMWARE
 * Maxime MOREILLON
 * 
 * Board type: ITEAD Sonoff S20
 * SSL support: basic
 */

#include "iotKernel.h"

#define DEVICE_TYPE "outlet"
#define DEVICE_FIRMWARE_VERSION "0.4.0"

// Pin mapping
#define RELAY_PIN 12
#define LED_PIN 13
#define BUTTON_PIN 0

IotKernel iot_kernel(DEVICE_TYPE,DEVICE_FIRMWARE_VERSION); 


void setup() {
  iot_kernel.init();
  IO_setup();
  iot_kernel.mqtt.setCallback(mqtt_message_callback);
}



void loop() {
 iot_kernel.loop();
 read_button();
}
