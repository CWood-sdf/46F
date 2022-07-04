#include "Odometry/PID.h"
#include "Odometry/GPS_Share.h"
#include "Bezier.h"
struct Chassis {
  double speedLimit = 100;
  double maxAcc = 500; // in/s^2
  double maxDAcc = 360; // in/s^2
  NewMotor<> leftWheels;
  NewMotor<> rightWheels;
  GPS_Share& pos;
  double trackWidth = 0.0;
  double gearRatio = 1.0;
  double wheelRad = 0.0;
  gearSetting cartridge = gearSetting::ratio18_1;
  double botAngle ();
  PVector& botPos();
  //converts pct to in/s
  double pctToReal(double speed){
    //rpm
    double motorVel;
    switch(cartridge){
      case vex::gearSetting::ratio18_1:
        motorVel = 200;
        break;
      case vex::gearSetting::ratio36_1:
        motorVel = 100;
        break;
      case vex::gearSetting::ratio6_1:
        motorVel = 600;
        break;
    }
    return motorVel * speed / 100.0 / gearRatio * 2.0 * M_PI / 60.0/*rad/s*/
           * wheelRad;
  }
  double realToPct(double speed){
    //rpm
    double motorVel;
    switch(cartridge){
      case vex::gearSetting::ratio18_1:
        motorVel = 200;
        break;
      case vex::gearSetting::ratio36_1:
        motorVel = 100;
        break;
      case vex::gearSetting::ratio6_1:
        motorVel = 600;
        break;
    }
    return speed / motorVel * 100.0 * gearRatio / 2.0 / M_PI * 60.0 / wheelRad;
  }
  void driveFromCurvature(double speed, double curvature);
  void driveFromAngular(double speed, double angularVel);
  void driveFromLR(double left, double right);
  void driveFromDiff(double speed, double diff, directionType d){
    leftWheels.spinVolt(d, speed + diff);
    rightWheels.spinVolt(d, speed - diff);
  }
  Chassis(GPS_Share& p, double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge);
};