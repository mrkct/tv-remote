
#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <EspMQTTClient.h>


class PowerManager {
  private:
  IRsend *irsend;
  EspMQTTClient *mqtt;

  long turnOffTime;
  boolean timerEnabled = false;
  
  public:
  PowerManager(IRsend*, EspMQTTClient*);
  void loop();

  void setTurnOffTimer(long timeFromNow);  
};

#endif
