#include <Arduino.h>
#include <Wire.h>
#include "Config.h"
#include "TiltSensor.h"


void TiltSensor::update() {
  Wire.beginTransmission(this->i2cAddr);
  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(this->i2cAddr, 6, true);
  this->accX = Wire.read()<<8|Wire.read();
  this->accY = Wire.read()<<8|Wire.read();
  this->accZ = Wire.read()<<8|Wire.read();
}

double TiltSensor::calculateAngle(double a, double b, double c) {
  double radians = atan2(
    a,
    sqrt((b*b) + (c*c))
  );
  // Convers from radians to degrees
  return radians * 180.0 / 3.14;
}

void TiltSensor::initialize() {
  Wire.begin(PIN_TILTSENSOR_SDA, PIN_TILTSENSOR_SCL);
  Wire.beginTransmission(this->i2cAddr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

double TiltSensor::getRoll() {
  this->update();
  return this->calculateAngle(
    this->accX,
    this->accY,
    this->accZ
  );
}

double TiltSensor::getPitch() {
  this->update();
  return this->calculateAngle(
    this->accY,
    this->accX,
    this->accZ
  );
}

boolean TiltSensor::isTiltedLeft() {
  return getPitch() < TILTSENSOR_LEFT_THRESHOLD;
}

boolean TiltSensor::isTiltedRight() {
  return getPitch() > TILTSENSOR_RIGHT_THRESHOLD;
}
