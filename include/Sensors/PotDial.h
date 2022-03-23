#ifndef POT_DIAL
#define POT_DIAL
#include "Odometry/EPA_Tracker.h"
typedef triport::port port;
class PotDial;
template<class StorageType>
class SelectorArr {
  friend class PotDial;
  static vector<PotDial*> boundDials;
  static vector<int> possibleTicks;
  map<vector<int>, StorageType> returnVals;
public:
  template<class ... Args>
  SelectorArr(Args&... potDials);
  SelectorArr(){

  }
  void addVal(vector<int> inputs, StorageType out){
    if(inputs.size() > possibleTicks.size()){
      cout << "Invalid array size (too big)" << endl;
    }
    else if(inputs.size() < possibleTicks.size()){
      cout << "Invalid array size (too small)" << endl;
    }
    else {
      returnVals[inputs] = out;
    }
  }
  StorageType getVal(vector<int> input){
    if(input.size() > possibleTicks.size()){
      cout << "Invalid array size (too big)" << endl;
    }
    else if(input.size() < possibleTicks.size()){
      cout << "Invalid array size (too small)" << endl;
    }
    return returnVals[input];
  }
  StorageType getVal();
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
  template<class T>
  PotDial& addController(SelectorArr<T>& ctrllr){
    ctrllr.possibleTicks.push_back(ticks);
    ctrllr.boundDials.push_back(this);
    return *this;
  }
  // template<typename ... Args>
  // PotDial(vex::triport::port& p, Args... otherArgs) : PotDial(otherArgs...) {
  //   sensor = new pot(p);
  // }
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
template<class T>
template<class ... Args>
SelectorArr<T>::SelectorArr(Args&... potDials){
  vector<Ref<PotDial>> dials = { potDials... };
  for(auto& d : dials){
    d.val->addController(*this);
  }
}
template<class T>
T SelectorArr<T>::getVal(){
  vector<int> vals = {};
  for(auto l : boundDials){
    vals.push_back(l->getAmnt());
  }
  return getVal(vals);
}
#endif