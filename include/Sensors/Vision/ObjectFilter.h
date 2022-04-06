#include "vex.h"
template<class T>
class BasicEMA {
  double alpha;
  T val;
  T oldVal;
public:
  BasicEMA(double alpha){
    this->alpha = alpha;
  }
  BasicEMA(double alpha, T val) : BasicEMA<T>(alpha){
    this->val = val;
  }
  void update(double sensorVal){
    val = sensorVal * alpha + (1 - alpha) * oldVal;
    oldVal = val;
  }
  T value (){
    return val;
  }
};
class EMA : public BasicEMA<double> {
public:
  using BasicEMA<double>::value;
  using BasicEMA<double>::update;
  EMA(double a) : BasicEMA<double>(a){

  }
  EMA(double a, double v) : BasicEMA<double>(a, v){

  }
};