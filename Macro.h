#ifndef MACRO_H
#define MACRO_H

#include "Config.h"

class Macro {
  private:
  int len;
  uint64_t commands[MAX_MACRO_SIZE];

  public:
  Macro();

  boolean loadFromEEPROM();
  void storeInEEPROM();
  
  void deleteMacro();
  int addCommand(uint64_t command);
  uint64_t getCommand(int);
  short getLength();
  bool isValid();
};

#endif
