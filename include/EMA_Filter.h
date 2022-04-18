#ifndef EMA_FILTER_H
#define EMA_FILTER_H
#include "vex.h"
template<class G, class T>
G converter_def(T val){
  return val;
}
template<class Get_T, class T = Get_T, Get_T (*converter)(T) = converter_def>
class BasicEMA {
  double alpha;
  T val;
  T oldVal;
public:
  BasicEMA(double alpha){
    this->alpha = alpha;
  }
  BasicEMA(double alpha, T val) : BasicEMA<T, Get_T>(alpha){
    this->val = val;
  }
  void update(T sensorVal){
    val = sensorVal * alpha + oldVal * (1 - alpha);
    oldVal = val;
  }
  Get_T value (){
    return converter(val);
  }
  operator Get_T(){
    return value();
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