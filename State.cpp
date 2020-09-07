#include "State.h"


static enum State currentState = STATE_DEFAULT;

void setState(enum State state) {
  currentState = state;
}

enum State getCurrentState() {
  return currentState;
}
