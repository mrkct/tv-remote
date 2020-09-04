#include <Arduino.h>
#include <EEPROM.h>
#include "Config.h"
#include "Macro.h"


Macro::Macro() { }

boolean Macro::loadFromEEPROM() {
  /**
   * Stored macros have a simple format. 
   * 2 byte magic and then a 0-terminated list of 64bit values representing ir codes
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
      this->len++;
    }
  } while (irCode != 0);

  return true;
}

void Macro::deleteMacro() {
  this->len = 0;  
}

int Macro::addCommand(uint64_t command) {
  if (this->len == MAX_MACRO_SIZE) {
    return -1;  
  }
  this->commands[this->len] = command;
  this->len++;
}

void Macro::storeInEEPROM() {
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

uint64_t Macro::getCommand(int index) {
  return this->commands[index];
}

short Macro::getLength() {
  return this->len;
}
