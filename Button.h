#ifndef BUTTON_H
#define BUTTON_H

enum Button {
  BTN_NONE, 
  BTN_POWER, 
  BTN_CHANNEL, 
  BTN_VOLUME, 
  BTN_PLAY_MACRO, 
  BTN_RECORD_MACRO, 
  BTN_TIMER15, 
  BTN_TIMER30, 
  BTN_TIMER1H
};

void setupButtons();

enum Button getPressedButton(void);

#endif
