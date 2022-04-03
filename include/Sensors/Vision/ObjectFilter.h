#include "vex.h"
class EMA {
  double alpha;
  double value;
  double oldValue;
  void update(double sensorVal){
    value = sensorVal * alpha + (1 - alpha) * oldValue;
    oldValue = value;
  }
};