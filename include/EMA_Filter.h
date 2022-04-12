#ifndef EMA_FILTER_H
#define EMA_FILTER_H
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
  void update(T sensorVal){
    val = sensorVal * alpha + oldVal * (1 - alpha);
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
#endif