#define NO_MAKE
#include "Odometry/Chassis.h"
PVector& Chassis::botPos(){
  return pos.position();
}
double Chassis::botAngle(){
  return pos.heading();
}
Chassis::Chassis(/*vector<Ref<motor>> left, vector<Ref<motor>> right, */GPS_Share& p, double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge) : pos(p){
  // leftWheels = left;
  // rightWheels = right;

  this->trackWidth = trackWidth;
  this->gearRatio = gearRatio;
  this->wheelRad = wheelRad;
  this->cartridge = cartridge;
}
void Chassis::turnRight(double speed){
  leftWheels.spinVolt(fwd, speed);
  rightWheels.spinVolt(reverse, speed);
}
void Chassis::turnLeft(double speed){
  leftWheels.spinVolt(reverse, speed);
  rightWheels.spinVolt(fwd, speed);
}
void Chassis::moveRightSide(double speed, directionType d){
  rightWheels.spinVolt(d, speed);
}
void Chassis::moveLeftSide(double speed, directionType d){
  leftWheels.spinVolt(d, speed);
}
void Chassis::hardBrake(){
  leftWheels.stop(hold);
  rightWheels.stop(hold);
}
void Chassis::coastBrake(){
  leftWheels.stop(coast);
  rightWheels.stop(coast);
}
Chassis::chain_method Chassis::setMaxAcc(double v){
  maxAcc = v;
  CHAIN
}
Chassis::chain_method Chassis::setMaxDAcc(double v){
  maxDAcc = v;
  CHAIN
}
Chassis::chain_method Chassis::setSpeedLimit(double v){
  speedLimit = v;
  CHAIN
}
