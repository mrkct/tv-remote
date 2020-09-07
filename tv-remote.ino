#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <EspMQTTClient.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>

#include "SAMSUNG_IR.h"
#include "Config.h"
#include "Button.h"
#include "Macro.h"
#include "PowerManager.h"
#include "State.h"


const uint16_t kCaptureBufferSize = 256;
const uint8_t kTimeout = 15;


IRsend irsend(PIN_IR_EMITTER);
IRrecv irrecv(PIN_IR_RECEIVER, kCaptureBufferSize, kTimeout, true);
decode_results results;

EspMQTTClient client(
  WIFI_SSID, 
  WIFI_PASSWORD, 
  MQTT_BROKER_IP, 
  MQTT_USERNAME, 
  MQTT_PASSWORD, 
  MQTT_CLIENT_NAME, 
  MQTT_PORT 
);

PowerManager powerManager(&irsend, &client);

// Used to avoid repeating too many times when a button is held
enum Button lastPressedBtn = BTN_NONE;
long lastPressedBtnTimestamp = 0;

Macro macro(&irsend, &irrecv, &results, &client);

MPU6050 accelgyro;
int16_t ax, ay, az, gx, gy, gz;

void setup() {
  setupButtons();
  Serial.begin(115200);
  irsend.begin();
  irrecv.enableIRIn();

  Wire.begin(PIN_MPU6050_SDA, PIN_MPU6050_SCL);
  accelgyro.initialize();

  client.enableDebuggingMessages();
}

void onConnectionEstablished() {
  Serial.println("Connection established");
  client.publish(MQTT_UPDATE_TOPIC, "{'event': 'hello'}", true);
}

void loop() {
  powerManager.loop();
  client.loop();
  macro.loop();

  if (getCurrentState() != STATE_DEFAULT) return;

  enum Button btn = getPressedButton();

  if (btn != BTN_NONE && btn == lastPressedBtn && millis() - lastPressedBtnTimestamp < BTN_DEBOUNCE_TIME) {
    return;
  }
  lastPressedBtn = btn;
  lastPressedBtnTimestamp = millis();

  if (btn == BTN_POWER) {
    Serial.println("Mando POWER");
    irsend.sendSAMSUNG(IR_POWER);
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'POWER'}", true);
  } else if (btn == BTN_CHANNEL) {
    Serial.println("Mando CHANNEL");
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    Serial.print("ay: ");
    Serial.println(ay);
    
    if (ay < -10000) {
      irsend.sendSAMSUNG(IR_CHANNEL_PREVIOUS);
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'CHANNEL-'}", true);
    } else if (ay > +10000) {
      irsend.sendSAMSUNG(IR_CHANNEL_NEXT);
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'CHANNEL+'}", true);
    }
  } else if (btn == BTN_VOLUME) {
    Serial.println("Mando VOLUME");
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    Serial.print("ax: ");
    Serial.println(ax);
    
    if (ax < -10000) {
      irsend.sendSAMSUNG(IR_VOLUME_DOWN);
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'VOLUME-'}", true);
    } else if (ax > +10000) {
      irsend.sendSAMSUNG(IR_VOLUME_UP);
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'VOLUME+'}", true);
    }
  } else if (btn == BTN_PLAY_MACRO) {
    Serial.println("Mando PLAY_MACRO");
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'PLAY_MACRO'}", true);
    if (!macro.isValid()) {
      Serial.println("There is no macro stored");
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'error', 'type': 'noMacroFound'}", true);
    } else {
      macro.playMacro();
    }
  } else if (btn == BTN_RECORD_MACRO) {
    Serial.println("Mando RECORD_MACRO");
    Serial.println("Recording a new macro...");
    macro.recordMacro();
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'RECORD_MACRO'}", true);
  } else if (btn == BTN_TIMER15) {
    Serial.println("Mando TIMER15");
    powerManager.setTurnOffTimer(1000 * 60 * 15);
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'timerSetup', 'duration': '15m'}", true);
  } else if (btn == BTN_TIMER30) {
    Serial.println("Mando TIMER30");
    powerManager.setTurnOffTimer(1000 * 60 * 30);
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'timerSetup', 'duration': '30m'}", true);
  } else if (btn == BTN_TIMER1H) {
    Serial.println("Mando TIMER1H");
    powerManager.setTurnOffTimer(1000 * 60 * 60);
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'timerSetup', 'duration': '1h'}", true);
  }
}
