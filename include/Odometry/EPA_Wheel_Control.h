//EPA_WheelControl.h -- Use this file to control the wheel base
//     to go to specified positions

//A few major ideas working in this file:
//  1 - Bezier Curves - To make beautifully smooth paths
//  2 - Pure Pursuit - To direct the bot's motion along Bezier paths
//  3 - PID Controllers - To direct the bot's speed along the path
#ifndef EPA_WHEEL_CONTROL_H
#define EPA_WHEEL_CONTROL_H

#include "Odometry/PID.h"
#include "Odometry/GPS_Share.h"
#include <deque>
#ifdef DEBUG
#warning "DEBUG already defined"
#else
#define DEBUG
#endif
#include "Bezier.h"
double sign(double v);
//The basic wheel controller 
class BasicWheelController {
protected: // PID variables + other random things
  // PosExtractor<gps> GPS;
  // PosExtractor Odom;
  typedef BasicWheelController& chain_method;
  LinkedList<PidAdder> customPidsDrive;
  LinkedList<PidAdder> customPidsTurn;
  LinkedList<PidAdder> customPidsSlave;
  GPS_Share& pos;
  bool usingGPS = true;
  //Cap, growth, zero
  PID slaveCtrl = PID(6.0, 0.1, 0.1, 0, 0, 10);
  //Get ctrl kP up to 6.25
  PID ctrl = PID(6.25, 0.001, 2.4325, 0, 8, 1);
  //Make derivative absolutely massive to force a hard stop
  //PID purePursuitCtrl = PID(5.5, 0.0, 3.4);
  PID turnCtrl = PID(2.42, 0.2, 1.35, 0, 20, 4);

 
  map<double, std::function<void()>> distFns, oldFns;
  bool callingInDrive = false;
public: // Some variables

  
  //A public path for drawing
  VectorArr path;
  bool drawArr = false;
  bool isOmniDir = false;
  bool hasFn = false;
  //Function to be called between turning and driving
  std::function<void()> afterTurn = [](){}; 
  NewMotor<> Left;
  NewMotor<> Right; 
public: // Constructor
  BasicWheelController(vector<Ref<motor>> L, vector<Ref<motor>> R, GPS_Share& s) :
    pos(s)
  {
    Left = L;
    Right = R;
  }
public: // Some Functions
  void addDrivePid(PidAdder a);
  void addDrivePid(double p, double i, double d);
  void popTopDrivePid();
  void addSlavePid(PidAdder a);
  void addSlavePid(double p, double i, double d);
  void popTopSlavePid();
  void addTurnPid(PidAdder a);
  void addTurnPid(double p, double i, double d);
  
  void driveTo(double x, double y);
  void backInto(double x, double y);
  virtual double botAngle ();
  PVector& botPos();
  //Add a function to be called at a specified distance
  void addDistFn(double dist, std::function<void()> fn);
  //Reuse the old map
  void reuseDistFns();
  void setFn(std::function<void()> fn);
  void callFn();
  void reuseFn();

  //A hard brake
  virtual void hardBrake();
public: // PID Stuff
  PID getCtrl();
  PID getSlave();
  void setOldDistFns();
  void useDistFns(double dist);
private: // turnTo, with re-updating function
  virtual void turnTo(std::function<double()> angleCalc);
public: // TurnTo
  double speedLimit = 100;
  virtual void turnTo(double angle);
  virtual void driveTo(PVector pos);
  enum class exitMode;
private: // followPath vars
  PVector lastTarget;
  double maxAcc = 500; // in/s^2
  double maxDAcc = 360; // in/s^2
  
  double kConst = 1.0;
  double exitDist = 0.0;
  exitMode BrakeMode = exitMode::normal;
  bool exitEarly = false;
  bool turnPrevent = false;
  bool reversed = false;
  bool moving = false;
  bool stopExitPrev = false;
  double pathRadius = 1.0;
public: // exitMode
  enum class exitMode {
    normal,
    hold = normal,
    coast,
    nothing
  };
public: // followPath var editors
  bool isMoving();
  chain_method estimateStartPos(PVector v, double a);
  void preventTurn();
  chain_method forceEarlyExit();
  chain_method setSpeedLimit(double v);
  chain_method setMaxAcc(double v);
  chain_method setMaxDeAcc(double v);
  chain_method setExitMode(exitMode m);
  chain_method setKConst(double v);
  chain_method setExitDist(double v);
  PVector getLastTarget();
  chain_method prevStopExit();
  chain_method setPathRadius(double r);
private: // General path follower

  size_t getNearest(VectorArr arr, PVector obj);
  size_t getNearest(VectorArr arr, PVector obj, size_t start);
  //The beefiest function in this file
  virtual void followPath(VectorArr arr, bool isNeg);
  virtual void ramseteTo(FieldCoord pose);
public: // Path following implementations
  virtual void followPath(VectorArr arr);
  virtual void backwardsFollow(VectorArr arr);
  virtual void backInto(PVector pos);
private: // Raw PID
  void basicPIDDrive(double targetDist, bool isNeg);
public: // Raw PID implementations
  virtual void driveFwd(double dist);
  virtual void driveBack(double dist);
public: // Functions that just move the wheels

  virtual void moveLeft(int speed, directionType d);
  virtual void moveRight(int speed, directionType d);
  virtual void turnLeft(int speed = 100);
  virtual void turnRight(int speed = 100);
  virtual void coastBrake();
  
};

class OmniWheelController : public BasicWheelController {
public: // Import variables + add constructor
  OmniWheelController(vector<Ref<motor>> BL, vector<Ref<motor>> BR, GPS_Share& s) :
    BasicWheelController(BL, BR, s)
    //positioner(positioner)
  {
    
  }
  
  OmniWheelController(motor& BL, motor& BR, GPS_Share& s) :
    BasicWheelController({BL}, {BR}, s)
  {
    
  }
};

class Omni_4Controller : public BasicWheelController{
public: // Import variables, functions + add constructor

  Omni_4Controller(vector<Ref<motor>> BL, vector<Ref<motor>> BR, GPS_Share& s) :
    BasicWheelController(BL, BR, s)
    //positioner(positioner)
  {
    
  }
  Omni_4Controller(motor& BL, motor& BR, motor& FL, motor& FR, GPS_Share& s) :
    BasicWheelController({BL, FL}, {BR, FR}, s)
  {
    
  }
};

class MechWheelController : public Omni_4Controller{
public: // Import variables + constructor
  motor* BL;
  motor* BR;
  motor* FL;
  motor* FR;
  bool isOmniDir = true;
  //MechWheelController(motor& BL, motor& BR, posTp&, gps&) = delete;
  MechWheelController(motor& BL, motor& BR, motor& FL, motor& FR, GPS_Share& s) :
    Omni_4Controller(BL, BR, FL, FR, s) {
    this->BL = &BL;
    this->BR = &BR;
    this->FL = &FL;
    this->FR = &FR;
  }

public: // EPIC PID Things, when actually used on competition bot, please rewrite
  void moveAt(double angle, double speed, double turnSpeed);
  void followPath(VectorArr arr, double targetAngle);
  void driveTo(PVector pos, double finalAngle);
  void driveTo(double x, double y, double finalAngle);
  void backInto(double x, double y, double finalAngle);
  void backInto(PVector pos, double finalAngle);
  virtual void backwardsFollow(VectorArr arr, double targetAngle);
};

class Omni_6Controller : public BasicWheelController{
public: // Import variables + constructor
  using BasicWheelController::botPos;
  bool isOmniDir = true;
  //Omni_6Controller(motor& BL, motor& BR, posTp&, gps&) = delete;
  //Omni_6Controller(motor& BL, motor& BR, motor&, motor&, posTp&, gps&) = delete;
  Omni_6Controller(vector<Ref<motor>> BL, vector<Ref<motor>> BR, GPS_Share& s) :
    BasicWheelController(BL, BR, s)
    //positioner(positioner)
  {
    
  }
  Omni_6Controller(motor& BL, motor& BR, motor& FL, motor& FR, motor& ML, motor& MR, GPS_Share& s) :
    BasicWheelController({BL, FL, ML}, {BR, FR, MR}, s)
  {
    
  }
};
#endif