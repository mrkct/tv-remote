#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>
/**
 * Edit this file with your wiring, tv model etc
 */

#define PIN_MUX1    D5
#define PIN_MUX2    D6
#define PIN_MUX3    D7
#define PIN_MUX_SIG D8

#define PIN_MPU6050_SDA D3
#define PIN_MPU6050_SCL D4
#define TILTSENSOR_LEFT_THRESHOLD -45
#define TILTSENSOR_RIGHT_THRESHOLD 45

#define MAX_MACRO_SIZE 16
#define MACRO_EEPROM_MAGIC 0xCAFE
#define EEPROM_ADDR_MACRO 0

#define BTN_DEBOUNCE_TIME 1500

#define PIN_IR_EMITTER D1
#define PIN_IR_RECEIVER D2

#define WIFI_SSID     "YOUR-WIFI-SSID-HERE"
#define WIFI_PASSWORD "YOUR-WIFI-PASSWORD-HERE"
// This is test.mosquitto.org 's server
#define MQTT_BROKER_IP    "5.196.95.208"
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD     ""
#define MQTT_CLIENT_NAME  "MyTVRemote" 
#define MQTT_PORT         1883
#define MQTT_UPDATE_TOPIC "mrkct/tvremote/updates"

#endif
