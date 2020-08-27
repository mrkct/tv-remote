#ifndef MACRO_H
#define MACRO_H

#include "Config.h"

class Macro {
  private:
  int len;
  uint64_t commands[MAX_MACRO_SIZE];

  public:
  Macro();

  void deleteMacro();
  int addCommand(uint64_t command);
  void storeMacro();
  uint64_t getCommand(int);
  short getLength();

  bool isValid();
};

#endif
