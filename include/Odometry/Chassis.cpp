#define NO_MAKE
#include "Chassis.h"
#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.0174532925199432957692369076842
#endif
PVector& Chassis::botPos(){
  #ifndef TEST
  return pos.position();
  #else
  return position.pos;
  #endif
}
double Chassis::botAngle(){
  #ifndef TEST
  return pos.heading();
  #else
  return position.angle / DEG_TO_RAD;
  #endif
}
#ifndef WINDOWS
Chassis::Chassis(NewMotor& left, NewMotor& right, GPS_Share& p, double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge) : pos(p), leftWheels(left), rightWheels(right){

  this->trackWidth = trackWidth;
  this->gearRatio = gearRatio;
  this->wheelRad = wheelRad;
}
#else
Chassis::Chassis(double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge) {
    //Set the wheel counts

	this->trackWidth = trackWidth;
	this->gearRatio = gearRatio;
	this->wheelRad = wheelRad;
	this->cartridge = cartridge;
}
#endif
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
  #ifndef TEST
  rightWheels.spinVolt(d, speed);
  #endif
}
void Chassis::moveLeftSide(double speed, directionType d){
  lastLeftSpeed = speed * (d == fwd ? 1 : -1);
  #ifndef TEST
  leftWheels.spinVolt(d, speed);
  #endif
}
void Chassis::hardBrake(){
  lastLeftSpeed = lastRightSpeed = 0;
  #ifndef TEST
  leftWheels.stop(hold);
  rightWheels.stop(hold);
  #else
  this->vel = this->angVel = 0.0;
  targetLeftVel = targetRightVel = 0;
  #endif

  
}
void Chassis::coastBrake(){
  lastLeftSpeed = lastRightSpeed = 0;
  #ifndef TEST
  leftWheels.stop(coast);
  rightWheels.stop(coast);
  #else
  this->vel = this->angVel = 0.0;
  targetLeftVel = targetRightVel = 0;
  #endif
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
double sign(double);
void Chassis::driveFromDiff(double speed, double diff, directionType d){
  if(abs(speed) > speedLimit){
    speed = speedLimit * sign(speed);
  }
  double left = speed + diff;
  double right = speed - diff;
  lastLeftSpeed = left;
  lastRightSpeed = right;
  cout << left << ", " << right << endl;
  #ifndef TEST
  leftWheels.spinVolt(left < 0 ? reverse : fwd, abs(left));
  rightWheels.spinVolt(right < 0 ? reverse : fwd, abs(right));
  #else
  targetLeftVel = min(max(static_cast<int>(speed + diff), -100), 100);
	targetRightVel = min(max(static_cast<int>(speed - diff), -100), 100);
  #endif
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
#ifdef TEST
void Chassis::runSimStep() {
    static int i = 0;
    double timeStep = static_cast<double>(stopwatch.getTime()) / 3.0;
    if (i++ < 19) {
        std::cout << timeStep << std::endl;
    }
    if (timeStep > 20) {
        timeStep = 10;
    }
    timeStep /= 1000.0;
    //Calculate the side accelleration from the target velocity and current velocity
    //      and torque and stuff
    //Calculate the current angular velocity (and maye the angular accelleration)
    //Update the position and velocity using Euler's method
    //Shift currentLeftVel
    double wheelAcc = 2;
    if (currentLeftVel < targetLeftVel && currentLeftVel < 100) {
        currentLeftVel += wheelAcc;
    }
    else if (currentLeftVel > targetLeftVel && currentLeftVel > -100) {
        currentLeftVel -= wheelAcc;
    }
    //Shift currentRightVel
    if (currentRightVel < targetRightVel && currentRightVel < 100) {
        currentRightVel += wheelAcc;
    }
    else if (currentRightVel > targetRightVel && currentRightVel > -100) {
        currentRightVel -= wheelAcc;
    }
    double realLeft = pctToReal(currentLeftVel);
double realRight = pctToReal(currentRightVel);
vel = (realLeft + realRight) / 2.0;
angVel = (realLeft - realRight) / trackWidth;
    auto fwd = this->vel * timeStep;
    this->position.pos.x += fwd * sin(-this->position.angle);
    this->position.pos.y += -fwd * cos(-this->position.angle);
    this->position.angle += this->angVel * timeStep;
    this->position.angle = posNeg180(position.angle / DEG_TO_RAD) * DEG_TO_RAD;
    if (i % 20 == 0) {
        std::cout << timeStep << "\n" << this->position.pos << "; " << this->position.angle << "\n"
            << currentLeftVel << ", " << currentRightVel << "\n"
            << targetLeftVel << ", " << targetRightVel << "\n" << std::endl;
    }
}
#endif