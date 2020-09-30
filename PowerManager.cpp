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
    this->mqtt->publish(MQTT_MESSAGES_TOPIC, "La TV è stata spenta", true);
  }

  /**
   * By default ESP.deepSleep, if the time is = 0, sleeps until a LOW signal 
   * is sent to the GPIO 16 (D0 on my board). 
   * If a timer is active, we need to wake up just before the timer goes off 
   * or we wont send the POWER signal
   */
  
  /*
  if (millis() - this->lastActionTime > SLEEP_TIMER) {
    long sleepTime = 0;
    if (this->timerEnabled) {
      sleepTime = this->turnOffTime - millis();
    }
    // Because it takes time in microseconds
    ESP.deepSleep(sleepTimer * 1000);
  }
  */
}

void PowerManager::setTurnOffTimer(long timeFromNow) {
  this->timerEnabled = true;
  this->turnOffTime = millis() + timeFromNow;


  char buffer[128];
  sprintf(buffer, "La TV si spegnerà tra %d minuti", timeFromNow / 1000 / 60);
  this->mqtt->publish(MQTT_MESSAGES_TOPIC, buffer, true); 
}
