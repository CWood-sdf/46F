#ifndef CHASSIS_H
#define CHASSIS_H
#include "PID.h"
#include "GPS_Share.h"
#include "Bezier.h"
#ifdef TEST
class Timer {
    std::chrono::milliseconds startTime = 
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
public:
    Timer() { reset(); }
	void reset() {
		startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
	}
    long long time() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch() - startTime).count();
    }
    long long time(timeUnits){
      return time();
    }
    operator long long() {
		return time();
    }
	
};
class Stopwatch {
    Timer timer;
public:
    Stopwatch() {
        timer.reset();
    }
    long long getTime() {
		auto time = timer.time();
        timer.reset();
        return time;
    }
	
};
#endif
struct Chassis {
  typedef Chassis& chain_method;
  #ifdef TEST
  Stopwatch stopwatch;
  FieldCoord position = {PVector(0, 0), 0};
  double vel = 0;
  double angVel = 0;
  double currentLeftVel = 0;
	double currentRightVel = 0;
	double targetLeftVel = 0;
	double targetRightVel = 0;
  #endif
  double speedLimit = 100;
  double maxAcc = 500; // in/s^2
  double maxDAcc = 360; // in/s^2
  #ifndef TEST
  NewMotor leftWheels;
  NewMotor rightWheels;
  vector<bool> ptoMotorsLeft = vector<bool>();
  vector<bool> ptoMotorsRight = vector<bool>();
  vector<pneumatics*> ptoPneumatics = vector<pneumatics*>();
  #endif
  double lastLeftSpeed = 0.0;
  double lastRightSpeed = 0.0;
  //If true, the chassis is on extra motors
  #ifndef TEST
  bool ptoEngaged = true;
  GPS_Share& pos;
  #endif
  double trackWidth = 0.0;
  double gearRatio = 1.0;
  double wheelRad = 0.0;
  gearSetting cartridge = gearSetting::ratio18_1;
  double botAngle();
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
  void driveFromDiff(double speed, double diff, directionType d);
  void hardBrake();
  void coastBrake();
  void turnLeft(double speed);
  void turnRight(double speed);
  void moveLeftSide(double speed, directionType d);
  void moveRightSide(double speed, directionType d);
  void move(double speed, directionType d);
  void engagePto();
  void disengagePto();
  chain_method setMaxAcc(double v);
  chain_method setMaxDAcc(double v);
  chain_method setSpeedLimit(double v);
  #ifndef TEST
  Chassis(vector<Ref<motor>> left, vector<Ref<motor>> right, GPS_Share& p, double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge);
  #else
  Chassis(double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge);
  void runSimStep() {
    
    cout << "Sdff" << endl; s(100);
        static int i = 0;
        
       cout << "Sdff" << endl; s(100);
        double timeStep = static_cast<double>(stopwatch.getTime()) / 3.0;
        
    cout << "Sdff" << endl; s(100);
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
};
#endif