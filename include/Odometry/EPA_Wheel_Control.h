//EPA_WheelControl.h -- Use this file to control the wheel base
//     to go to specified positions
//Todo: clean up stuff
#ifndef EPA_WHEEL_CONTROL_H
#define EPA_WHEEL_CONTROL_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include "Odometry/Controllers.h"
#include <deque>
#ifdef DEBUG
#warning "DEBUG already defined"
#else
#define DEBUG
#endif
//The basic wheel controller 
class BasicWheelController {
protected: // PID variables + other random things

  typedef BasicWheelController& chain_method;

  PID turnCtrl;

  map<double, std::function<void()>> distFns, oldFns;
  LinkedList<PidAdder> customPidsTurn;
  bool callingInDrive = false;
public: // Some variables

  
  //A public path for drawing
  VectorArr publicPath;
  bool drawArr = false;

  //Is true if afterTurn exists
  bool hasFn = false;
  //Function to be called between turning and driving
  std::function<void()> afterTurn = [](){}; 
  RamseteController* defaultRamsete;
  PurePursuitController* defaultPurePursuit;
  BasicPidController* defaultPid;
public: // Constructor
  BasicWheelController(Chassis* c, RamseteController* defRamsete, PurePursuitController* defPurePursuit, BasicPidController* defPid, PID turnCtrl, double kConst = 1.0)
  {
    defaultPurePursuit = defPurePursuit;
    defaultRamsete = defRamsete;
    defaultPid = defPid;
    path.setK(kConst);
    this->turnCtrl = turnCtrl;
    chassis = c;
  }
  Path path = Path();
public: // Some Functions
  // void addTurnPid(PidAdder a);
  // void addTurnPid(double p, double i, double d);
  // void popTopTurnPid();
  
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

  void setOldDistFns();
  void useDistFns(double dist);
private: // turnTo, with re-updating function
  virtual void turnTo(std::function<double()> angleCalc);
public: // TurnTo
  virtual void turnTo(double angle);
  enum class exitMode;
private: // followPath vars
  PVector lastTarget;
  double exitDist = 0.0;
  exitMode BrakeMode = exitMode::normal;
  bool exitEarly = false;
  bool reversed = false;
  bool moving = false;
  bool stopExitPrev = false;
  double pathRadius = 1.0;
  double followPathDist = 16.0;
  int followPathMaxTimeIn = 5;
public: // exitMode
  enum class exitMode {
    normal,
    hold = normal,
    coast,
    nothing
  };
  
public: // followPath var editors
  bool isMoving();
  double getPathRadius();
  double getFollowPathDist();
  chain_method setFollowPathDist(double dist);
  chain_method setFollowPathMaxTimeIn(int time);
  int getFollowPathMaxTimeIn();
  // chain_method setPathRadius(double r);
  chain_method estimateStartPos(PVector v, double a);
  chain_method forceEarlyExit();
  chain_method setExitMode(exitMode m);
  chain_method setExitDist(double v);
  PVector getLastTarget();
  chain_method prevStopExit();
  chain_method setPathRadius(double r);
private: // General path follower
  Chassis* chassis;
  template<class Arr>
  size_t getNearest(Arr arr, PVector obj);
  template<class Arr>
  size_t getNearest(Arr arr, PVector obj, size_t start);
public:
  virtual void ramseteFollow(VectorArr arr, bool isNeg);
  virtual void purePursuitFollow(VectorArr arr, bool isNeg);
  virtual void generalFollow(VectorArr arr, Controller* controller, bool isNeg);
  
};

class OmniWheelController : public BasicWheelController {
public: // Import variables + add constructor
  OmniWheelController(Chassis* c, RamseteController* ramsete, PurePursuitController* purePursuit, BasicPidController* defPid, PID tc, double kConst = 1.0) :
    BasicWheelController(c, ramsete, purePursuit, defPid, tc, kConst)
    //positioner(positioner)
  {
    
  }
};

class Omni_4Controller : public BasicWheelController{
public: // Import variables, functions + add constructor

  Omni_4Controller(Chassis* c, RamseteController* ramsete, PurePursuitController* purePursuit, BasicPidController* defPid, PID tc, double kConst = 1.0) :
    BasicWheelController(c, ramsete, purePursuit, defPid, tc, kConst)
    //positioner(positioner)
  {
    
  }
};

class MechWheelController : public Omni_4Controller{
public: // Import variables + constructor
  //MechWheelController(motor& BL, motor& BR, posTp&, gps&) = delete;
  MechWheelController(Chassis* c, RamseteController* ramsete, PurePursuitController* purePursuit, BasicPidController* defPid, PID tc, double kConst = 1.0) :
    Omni_4Controller(c, ramsete, purePursuit, defPid, tc, kConst) {

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
  Omni_6Controller(Chassis* c, RamseteController* ramsete, PurePursuitController* purePursuit, BasicPidController* defPid, PID tc, double kConst = 1.0) :
    BasicWheelController(c, ramsete, purePursuit, defPid, tc, kConst)
    //positioner(positioner)
  {
    
  }
};
#endif