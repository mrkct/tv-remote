#ifndef STATE_H
#define STATE_H

enum State {
  STATE_DEFAULT, 
  STATE_RECORDING_MACRO, 
  STATE_PLAYING_MACRO
};

void setState(enum State newState);

enum State getCurrentState();

#endif
