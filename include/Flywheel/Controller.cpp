#define NO_MAKE
#include "Flywheel/Controller.h"
FlywheelTBH::FlywheelTBH(NewMotor<> m, vex::triport::port& p) : en(p), filter(0.7){
  mots = m;
  filter.seed(0);
}
void FlywheelTBH::setTarget(int i){
  target = i;
  if(target < 0) 
    target = 0;
  else if(target >= velTargets.size()) 
    target = velTargets.size() - 1;
  tbh = initialTbh[target];
}
void FlywheelTBH::addTarget(double t){
  velTargets.push_back(t);
  initialTbh.push_back(0);
}
void FlywheelTBH::step(){
  static double lastRotation = 0;
  static int lastVel;
  static double lastDesiredVel = 0;
  static double prevErr = 0;
  int velSent = 10;
  bool calcTbh = true;
  double desiredVel = velTargets[target];
  int timeStep = velCheck.timeStep();
  double rotation = en.rotation(rotationUnits::rev);
  double speedEst = (rotation - lastRotation) / (double)timeStep * 60.0;
  lastRotation = rotation;
  filter.update(speedEst);
  double speed = filter.value();
  double err = speed - desiredVel;
  bool settled = velCheck.settled(err);
  if(settled){
    calcTbh = false;
  }
  if(desiredVel != lastDesiredVel){
    calcTbh = false;
  }
  lastDesiredVel = desiredVel;
  if(calcTbh){
    if(signbit(err) != signbit(prevErr)){
      tbh = (lastVel + tbh) / 2;
      if(tbh < 0){
        tbh = 0;
      }
      if(tbh > 100){
        tbh = 100;
      }
      initialTbh[target] = tbh;
      velSent = tbh;
    }
    else {
      velSent = lastVel + gain * err;
    }
  }
  if(velSent < 0){
    velSent = 0;
  }
  if(velSent > 100){
    velSent = 100;
  }
  prevErr = err;
  mots.spin(fwd, velSent);
  lastVel = velSent;
}


FlywheelPID::FlywheelPID(NewMotor<> m, vex::triport::port& p) : en(p), filter(0.7){
  mots = m;
  filter.seed(0);
}
void FlywheelPID::setTarget(int i){
  target = i;
  if(target < 0) 
    target = 0;
  else if(target >= velTargets.size()) 
    target = velTargets.size() - 1;
  ctrl.setTarget(velTargets[i]);
}
void FlywheelPID::step(){
  static double lastRotation = 0;
  double desiredVel = velTargets[target];
  int timeStep = velCheck.timeStep();
  double rotation = en.rotation(rotationUnits::rev);
  double speedEst = (rotation - lastRotation) / (double)timeStep * 60.0;
  lastRotation = rotation;
  filter.update(speedEst);
  double speed = filter.value();
  double err = speed - desiredVel;
  bool settled = velCheck.settled(err);

  mots.spin(fwd, ctrl.getVal(err));
}