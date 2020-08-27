#include <Arduino.h>
#include "Config.h"
#include "Buzzer.h"


void buzzerError() {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(1000);
  digitalWrite(PIN_BUZZER, LOW);
}

void buzzerSuccess() {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(600);
  digitalWrite(PIN_BUZZER, LOW);
}

void buzzerOk() {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(300);
  digitalWrite(PIN_BUZZER, LOW);
}
