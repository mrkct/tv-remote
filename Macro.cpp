#include <Arduino.h>
#include <EEPROM.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <EspMQTTClient.h>
#include "Config.h"
#include "Macro.h"
#include "State.h"
#include "Button.h"


Macro::Macro(IRsend *irsend, IRrecv *irrecv, decode_results *resultBuffer, EspMQTTClient *client) {
  this->irsend = irsend;
  this->irrecv = irrecv;
  this->resultBuffer = resultBuffer;
  this->mqtt = client;
}

boolean Macro::loadFromEEPROM() {
  /**
   * Stored macros have a simple format. 
   * 2 byte magic and then a 0-terminated list of 
   * 64bit values representing ir codes and 
   * 16bit value representing the time to wait before sending that IR code
   */
  int addr = EEPROM_ADDR_MACRO;
  uint16_t magic = EEPROM.read(addr) << 8 | EEPROM.read(addr+1);
  if (magic != MACRO_EEPROM_MAGIC) {
    return false;
  }
  
  uint64_t irCode;
  this->len = 0;
  addr += 2;
  do {
    // IRCodes are 64 bit values.
    irCode = 0;
    for (int i = 0; i < 8; i++) {
      irCode = irCode << 8 | EEPROM.read(addr + i);
    }
    addr += 8;
    
    if (irCode != 0) {
      this->commands[this->len] = irCode;
      
      uint16_t waitTime = EEPROM.read(addr) << 8 | EEPROM.read(addr + 1);
      addr += 2;   
      this->commandsWaitTimes[this->len] = waitTime;
   
      this->len++;
    }
  } while (irCode != 0);

  return true;
}

void Macro::storeInEEPROM() {
  Serial.println("Writing in EEPROM");
  int addr = EEPROM_ADDR_MACRO;
  EEPROM.write(addr, (MACRO_EEPROM_MAGIC >> 8) & 0xFF);
  EEPROM.write(addr + 1, MACRO_EEPROM_MAGIC & 0xFF);
  addr += 2;

  for (int i = 0; i < this->len; i++) {
    uint64_t code = this->commands[i];
    for (int i = 7; i >= 0; i--) {
      EEPROM.write(addr, (code >> (i * 8)) & 0xff);
      addr++;
    }

    uint16_t wait = this->commandsWaitTimes[i];
    for (int i = 1; i >= 0; i--) {
      EEPROM.write(addr, (wait >> (i * 8)) & 0xff);
      addr++;
    }
  }
  // Write the terminating 0
  for (int i = 0; i < 8; i++) {
    EEPROM.write(addr, 0);
    addr++;
  }

  EEPROM.commit();
}

bool Macro::isValid() {
  return this->len > 0;
}

void Macro::playMacro() {
  this->playingMacroIndex = 0;
  this->playingMacroNextDeadline = millis();
  setState(STATE_PLAYING_MACRO);
}

void Macro::recordMacro() {
  this->len = 0;
  setState(STATE_RECORDING_MACRO);
}

void Macro::playLoop() {
  if (getCurrentState() != STATE_PLAYING_MACRO || millis() < this->playingMacroNextDeadline) {
    return;
  }
  
  Serial.print("Sending another macro command (");
  Serial.print(this->playingMacroIndex);
  Serial.print("/");
  Serial.print(this->len - 1);
  Serial.print("). Next macro is in ");
  Serial.print(this->commandsWaitTimes[this->playingMacroIndex + 1]);
  Serial.println("ms");

  this->irsend->sendSAMSUNG(this->commands[this->playingMacroIndex]);
  
  this->playingMacroIndex++;
  this->playingMacroNextDeadline = millis() + this->commandsWaitTimes[this->playingMacroIndex];
  if (this->len == this->playingMacroIndex) {
    setState(STATE_DEFAULT);
  }
}

void Macro::recordLoop() {
  static uint64_t lastIRCode = 0;
  static long lastIRCodeTimestamp = 0;
  
  if (getCurrentState() != STATE_RECORDING_MACRO) {
    return;
  }

  if (getPressedButton() != BTN_RECORD_MACRO) {
    setState(STATE_DEFAULT);
    this->storeInEEPROM();
    return;
  }
    
  if (this->irrecv->decode(this->resultBuffer)) {
    Serial.println(resultToSourceCode(this->resultBuffer));
    uint64_t command = this->resultBuffer->value;
    long waitTime = millis() - lastIRCodeTimestamp;
    if (command == lastIRCode && waitTime < 500) {
      // This might be the original tv remote sending the same command many times
      Serial.println("I skipped this command because it probably was a repetition");
      return;
    }
    this->commands[this->len] = command;
    this->commandsWaitTimes[this->len] = (uint16_t) constrain(waitTime, 0, 60 * 1000);
    this->len++;
    lastIRCode = command;
    lastIRCodeTimestamp = millis();

    Serial.print("WaitTime=");
    Serial.println(waitTime);
    char buffer[200];
    sprintf(buffer, "Ricevuto codice %x, aspetto %hu", command, waitTime);
    this->mqtt->publish(MQTT_MESSAGES_TOPIC, buffer, true);
  }
}

void Macro::loop() {
  this->playLoop();
  this->recordLoop();
}
