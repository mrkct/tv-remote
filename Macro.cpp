#include <Arduino.h>
#include "Config.h"
#include "Macro.h"


Macro::Macro() {
  // Load from EEPROM
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

void Macro::storeMacro() {
  // Store in EEPROM
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
