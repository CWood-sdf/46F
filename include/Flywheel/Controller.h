#include "EMA_Filter.h"
#include "Odometry/PID.h"
#include <cstdint>
#include <algorithm>

//Determines if a flywheel is at the proper velocity
//Also calculates time between steps so that I don't have to 
//  Manage it elsewhere in the program
class Settled {
  double maxDeriv;
  timer time;
  double maxErr;
  double prevErr;
  double maxSleep = 500;
  uint32_t lastTimeStep;
public:
  Settled(double me, double md, double ms = 500){
    maxErr = me;
    maxDeriv = md;
    maxSleep = ms;
  }
  bool settled(double err){
    if(time.time() > 1000){
      return false;
    }
    if(time.time() < 30){
      return false;
    }
    lastTimeStep = time;
    uint32_t timeStep = time;
    double deriv = (err - prevErr) / (double)timeStep;
    return deriv < maxDeriv && err < maxErr;
  }
  uint32_t timeStep(){
    return lastTimeStep;
  }
};
struct FlywheelDebugEl {
  double error;
  double measuredVel;
  double filterVel;
  double targetVel;
  static const int size;
  void set(double err, double raw, double filtRetVal, double targ){
    error = err;
    measuredVel = raw;
    filterVel = filtRetVal;
    targetVel = targ;
  }
  //Sussy version
  void set(double arr[size]){
    for(int i = 0; i < size; i++){
      ((double*)this)[i] = arr[i];
    }
  }
};
struct FlywheelDebugLog {
  vector<FlywheelDebugEl> arr;
  void flush(){
    flushing = true;
    arr.clear();
    flushing = false;
  }
  bool flushing = false;
  void add(FlywheelDebugEl el){
    while(flushing){
      s(10);
    }
    arr.push_back(el);
  }
};


class Empty {
  public:
  virtual void step();
};
class FlywheelTBH : public Empty {
  encoder* en;
  NewMotor<> mots;
  EMA filter;
  vector<double> velTargets;
  vector<double> initialTbh;
  double tbh = 0;
  double gain = 80;
  Settled velCheck = Settled(100, 100 * 100, 500);
  int target;
  FlywheelDebugEl debug;
public:
  FlywheelTBH(NewMotor<> m, vex::encoder& e);
  void setTarget(int i);
  void addTarget(double t);
  void step() override;
  void graph(bool);
};
class EMA_D : public PIDF_Extension {
  EMA dFilter = EMA(0.7, 0);
public:
  void manageD(double &d) override {
    dFilter.update(d);
    d = dFilter.value();
  }
};
class FlywheelPID : public Empty {
  encoder* en;
  NewMotor<> mots;
  EMA filter;
  vector<double> velTargets;
  Settled velCheck = Settled(100, 100 * 100, 500);
  EMA_D manager;
  PIDF ctrl = PIDF(0.1, 0.1, 0.1, manager);
  FlywheelDebugEl debug;
  int target;
public:
  FlywheelPID(NewMotor<> m, vex::encoder& e);
  void setTarget(int i);
  void step() override;
  void graph(bool);
};
