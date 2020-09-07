#include <Arduino.h>
#include "Config.h"
#include "PowerManager.h"
#include "State.h"
#include "SAMSUNG_IR.h"


PowerManager::PowerManager(IRsend* irsend, EspMQTTClient* mqtt) {
  this->irsend = irsend;
  this->mqtt = mqtt;
}

void PowerManager::loop() {
  if (getCurrentState() != STATE_DEFAULT)
    return;
  
  if (this->timerEnabled && this->turnOffTime <= millis()) {
    Serial.println("Timer OFF");
    this->irsend->sendSAMSUNG(IR_POWER);
    this->mqtt->publish(MQTT_UPDATE_TOPIC, "{'event': 'timerOff'}", true);
    this->timerEnabled = false;
  }
}

void PowerManager::setTurnOffTimer(long timeFromNow) {
  this->timerEnabled = true;
  this->turnOffTime = millis() + timeFromNow;
}
