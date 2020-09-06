#ifndef TILTSENSOR_H
#define TILTSENSOR_H

class TiltSensor {
  private:

  const int i2cAddr = 0x68;
  int accX, accY, accZ;

  void update();
  double calculateAngle(double, double, double);


  public:

  void initialize();
  boolean isTiltedLeft();
  boolean isTiltedRight(); 

  double getTilt();

  double getRoll();
  double getPitch();

};

#endif
