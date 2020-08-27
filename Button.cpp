#include <Arduino.h>
#include "Button.h"


void setupButtons() {
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);
}


enum Button getPressedButton(void) {
  if (digitalRead(D6)) {
    return BTN_PLAY_MACRO;
  } else if (digitalRead(D7)) {
    return BTN_RECORD_MACRO;
  } else {
    return BTN_NONE;
  }
  // TODO: Leggi da multiplexer
}
