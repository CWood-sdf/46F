#define NO_MAKE
#include "Odometry/Chassis.h"
PVector& Chassis::botPos(){
  return pos.position();
}
double Chassis::botAngle(){
  return pos.heading();
}
Chassis::Chassis(GPS_Share& p, double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge) : pos(p){
  this->trackWidth = trackWidth;
  this->gearRatio = gearRatio;
  this->wheelRad = wheelRad;
  this->cartridge = cartridge;
}