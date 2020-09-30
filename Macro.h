#ifndef MACRO_H
#define MACRO_H

#include "Config.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <EspMQTTClient.h>

class Macro {
  private:
  int len;
  uint64_t commands[MAX_MACRO_SIZE];
  uint16_t commandsWaitTimes[MAX_MACRO_SIZE];

  short playingMacroIndex = 0;
  long playingMacroNextDeadline = 0;
  IRsend *irsend;
  IRrecv *irrecv;
  decode_results *resultBuffer;
  EspMQTTClient *mqtt;

  void playLoop();
  void recordLoop();
  
  public:
  Macro(IRsend*, IRrecv*, decode_results*, EspMQTTClient*);

  void playMacro();
  void recordMacro();
  
  void loop();

  boolean loadFromEEPROM();
  void storeInEEPROM();
  
  bool isValid();
};

#endif
