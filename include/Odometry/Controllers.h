#ifndef CONTROLLERS_H
#define CONTROLLERS_H
#include "Chassis.h"
#ifndef CHAIN
#define CHAIN return *this;
#endif
//That should be a sufficient path generator
class Path {
public:
  struct El {
    PVector bezierPt;
    double targetSpeed;
    double targetAngle;
    double curvature;
    operator PVector(){
      return bezierPt;
    }
  };
private:
  typedef Path& chain_method;
  vector<El> path;
  VectorArr arr;
  double kConst;
public:

  chain_method setK(double s);
  void make(VectorArr arr, Chassis* chassis);
  void remake(Chassis* chassis);
  VectorArr getBezier();
  vector<Ref<PVector>> getBezierRef();
  int size();
  El& last();
  El& operator[](int index);
};
double sign(double v);
class Controller {
  bool turnAtStart = false;
  bool defaultTurn = true;
public:  
  bool isTurnAtStart(){
    return turnAtStart;
  }
  void setTurn(){
    turnAtStart = true;
  }
  void preventTurn(){
    turnAtStart = false;
  }
  void setDefaultTurn(bool val){
    defaultTurn = val;
  }
  bool getDefaultTurn(){
    return defaultTurn;
  }
  struct Input {
    PVector target;
    PVector position;
    double angleTarget;
    double currentAngle;
    double dist;
    Path::El targetPt;
    Chassis* chassis;
    //bool isNeg;

  };
  enum class AngularVel {
    curvature,
    pctDiff,
    radps
  };
  enum class ForwardVel {
    pct,
    inps
  };
  
  virtual void init();
  //Pure Pursuit
  typedef pair<pair<double, ForwardVel>, pair<double, AngularVel>> followToRet;
  virtual followToRet followTo(Input& input){}
  followToRet followTo(Input&& v){
    return followTo(v);
  }
  
  virtual void deInit();
  Controller(){

  }
};
class PurePursuitController : public Controller {
  //Get ctrl kP up to 6.25
  PID ctrl;
public:
  followToRet followTo(Input& input) override;
  void init() override;
  template<class T, class... Args>
  PurePursuitController(T val, Args... args){
    ctrl = PID(val, args...);
  }
  PurePursuitController(PID input);
  PurePursuitController() = delete;
};
class RamseteController : public Controller {
  //beta ~ p-term 
  //[b > 0]
  //zeta ~ d-term
  //[0 < zeta < 1]
  double beta;//rad^2/in^2
  double zeta;//1/rad
public:
  followToRet followTo(Input &input) override;
  RamseteController(double beta, double zeta);
  RamseteController() = delete;
};
class BasicPidController : public Controller {
  PID ctrl;
  PID slave;
public:
  void init() override;
  BasicPidController(PID, PID);
  followToRet followTo(Input &input) override;
  BasicPidController() = delete;
};
#endif