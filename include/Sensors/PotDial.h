#ifndef POT_DIAL
#define POT_DIAL
#include "Odometry/EPA_Tracker.h"
typedef triport::port port;
class PotDial;
class SelectorArr {
  typedef int FnTp;
  static inline FnTp emptyFn = 0;
  static vector<PotDial*> boundDials;
  static map<vector<int>, FnTp> toVal;
  vector<int> count;
  bool isValid = false;
  friend class PotDial;
  static void addDial(PotDial* dial){
    for(auto p : boundDials){
      //Dial already contained in the array, abort
      if(p == dial){
        return ;
      }
    }
    boundDials.push_back(dial);
    if(toVal.size() > 0){
      cout << "In SelectorArr: "
           << "Adding new dial, deleting " << toVal.size() << " output item(s)" << endl;
      toVal.clear();
    }
  }
public:
  SelectorArr(vector<int> count, FnTp fn){
    if(count.size() != boundDials.size()){
      cout 
        << "Invalid input size, given array size (" 
        << count.size() 
        << ") does not match dial count of (" 
        << boundDials.size() 
        << "), aborting init of SelectorArr(vector<int>, FnTp)" 
        << endl;
    }
    else {
      isValid = true;
      toVal[count] = fn;
      this->count = count;
    }
  }
  SelectorArr(vector<int> count){
    if(count.size() != boundDials.size()){
      cout 
        << "Invalid input size, given array size (" 
        << count.size() 
        << ") does not match dial count of (" 
        << boundDials.size() 
        << "), aborting init of SelectorArr(vector<int>)" 
        << endl;
    }
    else {
      isValid = true;
      toVal[count] = emptyFn;
    }
  }
  SelectorArr() = delete;
  void attachFn(FnTp fn){
    if(isValid){
      toVal[count] = fn;
    }
  }
  static FnTp getVal();
};
class PotDial {
  const int ticks;
  pot* sensor;
  double baseVal = 0.0;
  int range = 250;
  
  PotDial(int tickAmnt) : ticks(tickAmnt) {
    SelectorArr::addDial(this);
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
SelectorArr::FnTp SelectorArr::getVal(){
  vector<int> vals;
  for(auto l : boundDials){
    vals.push_back(l->getAmnt());
  }
  if(toVal.count(vals) == 1){
    return toVal[vals];
  }
  //Err msg: show inputs received by prog, show possible inputs
  cout << "In SelectorArr: ";
  cout << "Input array of {";
  for(int i = 0; i < vals.size(); i++){
    cout << vals[i];
    if(i != vals.size() - 1){
      cout << ", ";
    }
  }
  cout << "} does not match any of the declared inputs: \n";
  for(auto& [key, val] : toVal){
    cout << "  {";
    for(int i = 0; i < key.size(); i++){
    cout << key[i];
      if(i != key.size() - 1){
        cout << ", ";
      }
    }
    cout << "}\n";
  }
  cout << flush;
  return emptyFn;
}
#endif