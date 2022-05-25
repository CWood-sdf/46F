#include "EMA_Filter.h"
#include "Odometry/PID.h"
#include <cstdint>
#include <algorithm>
//Flywheel with TBH control
class Settled {
  double maxDeriv;
  uint32_t lastTime = 0;
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
    if(Brain.Timer.system() - lastTime > 1000){
      return false;
    }
    auto time = Brain.Timer.system();
    if(time < 30){
      return false;
    }
    lastTimeStep = time;
    uint32_t timeStep = time - lastTime;
    double deriv = (err - prevErr) / (double)timeStep;
    return deriv < maxDeriv && err < maxErr;
  }
  uint32_t timeStep(){
    return lastTimeStep;
  }
};
class FlywheelTBH {
  encoder en;
  NewMotor<> mots;
  EMA filter;
  vector<double> velTargets;
  vector<double> initialTbh;
  double tbh = 0;
  double gain = 80;
  Settled velCheck = Settled(100, 100 * 100, 500);
  int target;
  struct {
    double error;
    double measuredVel;
    double filterVel;
    double targetVel;
  } debug;
public:
  FlywheelTBH(NewMotor<> m, vex::triport::port& p);
  void setTarget(int i);
  void addTarget(double t);
  void step();
};
class EMA_D : public PIDF_Extension {
  EMA dFilter = EMA(0.7, 0);
public:
  void manageD(double &d) override {
    dFilter.update(d);
    d = dFilter.value();
  }
};
class FlywheelPID {
  encoder en;
  NewMotor<> mots;
  EMA filter;
  vector<double> velTargets;
  Settled velCheck = Settled(100, 100 * 100, 500);
  EMA_D manager;
  PIDF ctrl = PIDF(0.1, 0.1, 0.1, manager);
  int target;
public:
  FlywheelPID(NewMotor<> m, vex::triport::port& p);
  void setTarget(int i);
  void step();
};
