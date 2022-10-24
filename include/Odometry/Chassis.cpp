#define NO_MAKE
#include "Odometry/Chassis.h"
PVector& Chassis::botPos(){
  return pos.position();
}
double Chassis::botAngle(){
  return pos.heading();
}
Chassis::Chassis(vector<Ref<motor>> left, vector<Ref<motor>> right, GPS_Share& p, double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge) : pos(p){
  leftWheels = left;
  rightWheels = right;

  this->trackWidth = trackWidth;
  this->gearRatio = gearRatio;
  this->wheelRad = wheelRad;
  this->cartridge = cartridge;
}
void Chassis::engagePto(){
  
}
void Chassis::disengagePto(){
  
}
void Chassis::turnRight(double speed){
  driveFromDiff(0, speed, fwd);
}
void Chassis::turnLeft(double speed){
  driveFromDiff(0, -speed, fwd);
}
void Chassis::moveRightSide(double speed, directionType d){
  lastRightSpeed = speed * (d == fwd ? 1 : -1);
  rightWheels.spinVolt(d, speed);
}
void Chassis::moveLeftSide(double speed, directionType d){
  lastLeftSpeed = speed * (d == fwd ? 1 : -1);
  leftWheels.spinVolt(d, speed);
}
void Chassis::hardBrake(){
  lastLeftSpeed = lastRightSpeed = 0;
  leftWheels.stop(hold);
  rightWheels.stop(hold);
  
}
void Chassis::coastBrake(){
  lastLeftSpeed = lastRightSpeed = 0;
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
void Chassis::driveFromDiff(double speed, double diff, directionType d){
  double left = speed + diff;
  double right = speed - diff;
  lastLeftSpeed = left;
  lastRightSpeed = right;
  leftWheels.spinVolt(left < 0 ? reverse : fwd, abs(left));
  rightWheels.spinVolt(right < 0 ? reverse : fwd, abs(right));
  
  
}
//Speed is in pct, velocity is in rad/sec
void Chassis::driveFromAngular(double speed, double vel) {
	driveFromCurvature(speed, 1 / pctToReal(speed) * vel);
}
//Speed is in pct, curvature is 1/in
void Chassis::driveFromCurvature(double speed, double curvature) {
	double realSpeed, leftSpeed, rightSpeed;
  int count = 0;
	TOP:
	realSpeed = pctToReal(speed);
	leftSpeed = realToPct(realSpeed * (2.0 + curvature * trackWidth) / 2.0);
	rightSpeed = realToPct(realSpeed * (2.0 - curvature * trackWidth) / 2.0);
	if ((abs(leftSpeed) > 100 || abs(rightSpeed) > 100) && realToPct(abs(curvature * trackWidth)) < 100 && count++ < 5) {
		double max = std::max(leftSpeed, rightSpeed);
    if(max < 100){
      max = std::min(leftSpeed, rightSpeed);
    }
		speed *= 100 / max;
		goto TOP;
	}
	driveFromLR(leftSpeed, rightSpeed);
}
void Chassis::driveFromLR(double left, double right) {
	double diff = (left - right) / 2.0;
	driveFromDiff((left + right) / 2.0, diff, fwd);
}