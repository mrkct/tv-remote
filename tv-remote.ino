#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "SAMSUNG_IR.h"
#include "Config.h"
#include "Button.h"
#include "Macro.h"
#include "TiltSensor.h"
#include "EspMQTTClient.h"


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

// Used to avoid repeating too many times when a button is held
enum Button lastPressedBtn = BTN_NONE;
long lastPressedBtnTimestamp = 0;

// If this is != 0, then when millis >= turnOffTimer a Power signal will be sent
long turnOffTimer = 0;

Macro macro;
short playingMacroIndex = 0;
long playingMacroNextDeadline = 0;

TiltSensor tiltSensor;

enum State {
  STATE_DEFAULT, 
  STATE_RECORDING_MACRO, 
  STATE_PLAYING_MACRO
} currentState = STATE_DEFAULT;

void setup() {
  setupButtons();
  Serial.begin(115200);
  irsend.begin();
  irrecv.enableIRIn();
  tiltSensor.initialize();

  client.enableDebuggingMessages();
}

void onConnectionEstablished() {
  Serial.println("Connection established");
  client.publish(MQTT_UPDATE_TOPIC, "{'event': 'hello'}", true);
}

void loop() {
  client.loop();
  if (currentState == STATE_RECORDING_MACRO) {
    if (getPressedButton() != BTN_RECORD_MACRO) {
      currentState = STATE_DEFAULT;
      Serial.println("Macro stored. This is what I have: ");

      String out = "";
      for (int i = 0; i < macro.getLength(); i++) {
        out += uint64ToString(macro.getCommand(i), 16);
        out += " ";
      }
      Serial.println(out);
      return;
    }
    
    if (irrecv.decode(&results)) {
      Serial.println(resultToSourceCode(&results));
      macro.addCommand(results.value);
    }
  } else if (currentState == STATE_PLAYING_MACRO) {
    if (millis() < playingMacroNextDeadline) {
      return;
    }
    Serial.println("Sending " + uint64ToString(macro.getCommand(playingMacroIndex), 16));
    irsend.sendSAMSUNG(macro.getCommand(playingMacroIndex));
    
    playingMacroIndex++;
    playingMacroNextDeadline = millis() + 1500;
    if (macro.getLength() == playingMacroIndex) {
      currentState = STATE_DEFAULT;
    }
  } 

  if (currentState != STATE_DEFAULT) return;

  if (turnOffTimer != 0 && turnOffTimer <= millis()) {
    Serial.println("Timer OFF");
    irsend.sendSAMSUNG(IR_POWER);
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'timerOff'}", true);
  }
  
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
    Serial.print(tiltSensor.getRoll());
    Serial.print("\t");
    Serial.println(tiltSensor.getPitch());
    Serial.println("Mando CHANNEL");
    if (tiltSensor.isTiltedLeft()) {
      irsend.sendSAMSUNG(IR_CHANNEL_PREVIOUS);
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'CHANNEL-'}", true);
    } else if (tiltSensor.isTiltedRight()) {
      irsend.sendSAMSUNG(IR_CHANNEL_NEXT);
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'CHANNEL+'}", true);
    }
  } else if (btn == BTN_VOLUME) {
    Serial.println("Mando VOLUME");
    if (tiltSensor.isTiltedLeft()) {
      irsend.sendSAMSUNG(IR_VOLUME_DOWN);
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'VOLUME-'}", true);
    } else if (tiltSensor.isTiltedRight()) {
      irsend.sendSAMSUNG(IR_VOLUME_UP);
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'VOLUME+'}", true);
    }
  } else if (btn == BTN_PLAY_MACRO) {
    Serial.println("Mando PLAY_MACRO");
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'PLAY_MACRO'}", true);
    if (!macro.isValid()) {
      Serial.println("There is no macro stored");
      client.publish(MQTT_UPDATE_TOPIC, "{'event': 'noMacroFound'}", true);
    } else {
      currentState = STATE_PLAYING_MACRO;
      playingMacroIndex = 0;
      playingMacroNextDeadline = millis();
    }
  } else if (btn == BTN_RECORD_MACRO) {
    Serial.println("Mando RECORD_MACRO");
    Serial.println("Recording a new macro...");
    currentState = STATE_RECORDING_MACRO;
    macro.deleteMacro();
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'button', 'button': 'RECORD_MACRO'}", true);
  } else if (btn == BTN_TIMER15) {
    Serial.println("Mando TIMER15");
    turnOffTimer = millis() + 1000 * 60 * 15;
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'timerSetup', 'duration': '15m'}", true);
  } else if (btn == BTN_TIMER30) {
    Serial.println("Mando TIMER30");
    turnOffTimer = millis() + 1000 * 60 * 30;
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'timerSetup', 'duration': '30m'}", true);
  } else if (btn == BTN_TIMER1H) {
    Serial.println("Mando TIMER1H");
    turnOffTimer = millis() + 1000 * 60 * 60;
    client.publish(MQTT_UPDATE_TOPIC, "{'event': 'timerSetup', 'duration': '1h'}", true);
  }
}
