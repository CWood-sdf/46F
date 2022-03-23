#ifndef POT_DIAL
#define POT_DIAL
#include "Odometry/EPA_Tracker.h"
typedef triport::port port;
class PotDial;
class SelectorArr {
  std::function<int()> val;
  static vector<PotDial*> boundDials;
  static map<vector<int>, SelectorArr*> toVal;
public:

};
class PotDial {
  const int ticks;
  pot* sensor;
  double baseVal = 0.0;
  int range = 250;
  PotDial(int tickAmnt) : ticks(tickAmnt) {

  }
  PotDial(int tickAmnt, int rng) : PotDial(tickAmnt) {
    range = rng;
  }
  PotDial(int tickAmnt, int rng, double baseVal) : PotDial(tickAmnt, rng) {
    this->baseVal = baseVal;
  }
public:
  
public:
  template<typename ... Args>
  PotDial(pot& sensor, Args... otherArgs) : PotDial(otherArgs...){
    this->sensor = &sensor;
  }
  template<typename... Args>
  PotDial(port& p, Args... args) : PotDial(args...){
    sensor = new pot(p);
  }
  int getAmnt(){
    double angle = sensor->angle(deg);
    if(angle <= baseVal){
      return 1;
    } else if(angle >= baseVal + range){
      return ticks;
    } else {
      angle -= baseVal;
      angle /= (double)range;
      angle *= (double)ticks - 1;
      return round(angle) + 1;
    }
  }
};

#endif