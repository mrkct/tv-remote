#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>
/**
 * Edit this file with your wiring, tv model etc
 */

#define PIN_MUX1    D4
#define PIN_MUX2    D5
#define PIN_MUX3    D6
#define PIN_MUX4    D7
#define PIN_MUX_SIG D8

#define PIN_TILTSENSOR_SDA D2
#define PIN_TILTSENSOR_SCL D1
#define TILTSENSOR_LEFT_THRESHOLD -45
#define TILTSENSOR_RIGHT_THRESHOLD 45

#define MAX_MACRO_SIZE 16
#define MACRO_EEPROM_MAGIC 0xCAFE
#define EEPROM_ADDR_MACRO 0

#define BTN_DEBOUNCE_TIME 1500

#define PIN_IR_EMITTER D0
#define PIN_IR_RECEIVER D1

#endif
