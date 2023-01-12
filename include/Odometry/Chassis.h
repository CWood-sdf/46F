#ifndef CHASSIS_H
#define CHASSIS_H
#include "Bezier.h"
#include "GPS_Share.h"
#include "PID.h"
#ifdef TEST
class Timer
{
  std::chrono::milliseconds startTime =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch());

  public:
  Timer() { reset(); }
  void reset()
  {
    startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }
  long long time()
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch() - startTime)
        .count();
  }
  long long time(timeUnits)
  {
    return time();
  }
  operator long long()
  {
    return time();
  }
};
class Stopwatch
{
  Timer timer;

  public:
  Stopwatch()
  {
    timer.reset();
  }
  long long getTime()
  {
    auto time = timer.time();
    timer.reset();
    return time;
  }
};
#endif
struct Chassis;
class Controller;
class Path
{
  public:
  struct El
  {
    PVector bezierPt;
    double targetSpeed;
    double targetAngle;
    double curvature;
    operator PVector()
    {
      return bezierPt;
    }
  };

  private:
  typedef Path &chain_method;
  vector<El> path;
  VectorArr arr;
  double kConst;

  public:
  chain_method setK(double s);
  void make(VectorArr &arr, Chassis *chassis);
  void remake(Chassis *chassis);
  VectorArr getBezier();
  vector<Ref<PVector>> getBezierRef();
  int size();
  El &last();
  El &operator[](int index);
};
struct Chassis
{
  typedef Chassis &chain_method;
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
  double maxAcc = 100; // in/s^2
  double maxDAcc = 80; // in/s^2
#ifndef WINDOWS
  NewMotor &leftWheels;
  NewMotor &rightWheels;
  vector<bool> ptoMotorsLeft = vector<bool>();
  vector<bool> ptoMotorsRight = vector<bool>();
  vector<pneumatics *> ptoPneumatics = vector<pneumatics *>();
#endif
  double lastLeftSpeed = 0.0;
  double lastRightSpeed = 0.0;
// If true, the chassis is on extra motors
#ifndef WINDOWS
  bool ptoEngaged = true;
  GPS_Share &pos;
#endif
  double trackWidth = 0.0;
  double gearRatio = 1.0;
  double wheelRad = 0.0;
  gearSetting cartridge = gearSetting::ratio18_1;
  double botAngle();
  PVector &botPos();
  double revToInches(double rotation)
  {
    return rotation * 2.0 * M_PI * wheelRad / gearRatio;
  }
  double inchesToRev(double inches)
  {
    return inches * gearRatio / 2.0 / M_PI / wheelRad;
  }
  // converts pct to in/s
  double pctToReal(double speed)
  {
    // rpm
    double motorVel;
    switch (cartridge)
    {
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
    return motorVel * speed / 100.0 / gearRatio * 2.0 * M_PI / 60.0 /*rad/s*/
           * wheelRad;
  }
  double realToPct(double speed)
  {
    // rpm
    double motorVel;
    switch (cartridge)
    {
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
#ifndef WINDOWS
  Chassis(NewMotor &left, NewMotor &right, GPS_Share &p, double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge);
#else
  Chassis(double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge);
#endif
#ifdef TEST
  void runSimStep();
#endif
};
#endif