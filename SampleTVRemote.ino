#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "SAMSUNG_IR.h"
#include "Config.h"
#include "Button.h"
#include "Macro.h"
// #include "Buzzer.h"
// #include "MQTTTracker.h"

const uint16_t kCaptureBufferSize = 256;
const uint8_t kTimeout = 15;


IRsend irsend(PIN_IR_EMITTER);
IRrecv irrecv(PIN_IR_RECEIVER, kCaptureBufferSize, kTimeout, true);
decode_results results;

void setup() {
  setupButtons();
  Serial.begin(115200);
  irsend.begin();
  irrecv.enableIRIn();
  // load macro from eeprom
}

// Used to avoid repeating too many times when a button is held
enum Button lastPressedBtn = BTN_NONE;
long lastPressedBtnTimestamp = 0;

// If this is != 0, then when millis >= turnOffTimer a Power signal will be sent
long turnOffTimer = 0;

Macro macro;
short playingMacroIndex = 0;
long playingMacroNextDeadline = 0;

enum State {
  STATE_DEFAULT, 
  STATE_RECORDING_MACRO, 
  STATE_PLAYING_MACRO
};
enum State currentState = STATE_DEFAULT;


void loop() {
  if (currentState == STATE_RECORDING_MACRO) {
    if (getPressedButton() != BTN_RECORD_MACRO) {
      currentState = STATE_DEFAULT;
      Serial.println("Macro stored. This is what I have: ");

      String out = "";
      for (int i = 0; i < macro.getLength(); i++) {
        out += uint64ToString(macro.getCommand(i), 16);
        out += " ";
      }
      Serial.println(out);
      // Store macro in EEPROM
      // Buzzer success
      return;
    }
    
    if (irrecv.decode(&results)) {
      Serial.println(resultToSourceCode(&results));
      macro.addCommand(results.value);
    }
  } else if (currentState == STATE_PLAYING_MACRO) {
    if (millis() < playingMacroNextDeadline) {
      return;
    }
    Serial.println("Sending " + uint64ToString(macro.getCommand(playingMacroIndex), 16));
    irsend.sendSAMSUNG(macro.getCommand(playingMacroIndex));
    
    playingMacroIndex++;
    playingMacroNextDeadline = millis() + 1500;
    if (macro.getLength() == playingMacroIndex) {
      currentState = STATE_DEFAULT;
    }
  } 

  if (currentState != STATE_DEFAULT) return;

  if (turnOffTimer != 0 && turnOffTimer <= millis()) {
    Serial.println("Timer OFF");
    irsend.sendSAMSUNG(IR_POWER);
  }
  
  enum Button btn = getPressedButton();

  if (btn != BTN_NONE && btn == lastPressedBtn && millis() - lastPressedBtnTimestamp < BTN_DEBOUNCE_TIME) {
    return;
  }
  lastPressedBtn = btn;
  lastPressedBtnTimestamp = millis();
  // sendButtonMQTT(btn);

  if (btn == BTN_POWER) {
    Serial.println("Mando POWER");
    irsend.sendSAMSUNG(IR_POWER);
  } else if (btn == BTN_CHANNEL) {
    
  } else if (btn == BTN_VOLUME) {
    // Check tilt and send + or - accordingly
  } else if (btn == BTN_PLAY_MACRO) {
    if (!macro.isValid()) {
      Serial.println("There is no macro stored");
    } else {
      currentState = STATE_PLAYING_MACRO;
      playingMacroIndex = 0;
      playingMacroNextDeadline = millis();
    }
  } else if (btn == BTN_RECORD_MACRO) {
    Serial.println("Recording a new macro...");
    currentState = STATE_RECORDING_MACRO;
    macro.deleteMacro();
  } else if (btn == BTN_TIMER15) {
    turnOffTimer = millis() + 1000 * 60 * 15;  
  } else if (btn == BTN_TIMER30) {
    turnOffTimer = millis() + 1000 * 60 * 30;
  } else if (btn == BTN_TIMER1H) {
    turnOffTimer = millis() + 1000 * 60 * 60;
  }
}
