#include <Arduino.h>
#include "Button.h"
#include "Config.h"


void setupButtons() {
  pinMode(PIN_MUX1, OUTPUT);
  pinMode(PIN_MUX2, OUTPUT);
  pinMode(PIN_MUX3, OUTPUT);
  pinMode(PIN_MUX4, OUTPUT);
  pinMode(PIN_MUX_SIG, INPUT);
}

/**
 * Sets the multiplexer's channel pins based on the index of the 
 * channel to read.
 * Param:
 *  index: An integer between 0 and 7 (included)
 */
static void setMuxChannel(int index) {
  static byte states[16][4] = {
    {0, 0, 0, 0}, 
    {0, 0, 0, 1}, 
    {0, 0, 1, 0}, 
    {0, 0, 1, 1}, 
    {0, 1, 0, 0}, 
    {0, 1, 0, 1}, 
    {0, 1, 1, 0}, 
    {0, 1, 1, 1}
  };
  digitalWrite(PIN_MUX1, states[index][3] ? HIGH: LOW);
  digitalWrite(PIN_MUX2, states[index][2] ? HIGH: LOW);
  digitalWrite(PIN_MUX3, states[index][1] ? HIGH: LOW);
  digitalWrite(PIN_MUX4, states[index][0] ? HIGH: LOW);
}


enum Button getPressedButton(void) {
  static enum Button buttons[] = {
    BTN_POWER, 
    BTN_CHANNEL, 
    BTN_VOLUME, 
    BTN_PLAY_MACRO, 
    BTN_RECORD_MACRO, 
    BTN_TIMER15, 
    BTN_TIMER30, 
    BTN_TIMER1H
  };
  for (int i = 0; i < 8; i++) {
    setMuxChannel(i);
    delay(4); // Necessary, or we read the previous channel
    if (digitalRead(PIN_MUX_SIG))
      return buttons[i];
  }
  return BTN_NONE;
}
