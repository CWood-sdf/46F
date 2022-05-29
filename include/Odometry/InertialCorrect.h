#ifndef INERTIAL_CORRECT_H
#define INERTIAL_CORRECT_H
#include "vex.h"
class Inertial {
  inertial* i;
public:
  Inertial(inertial& in) : i(&in){

  }
  Inertial(int32_t port) : i(new inertial(port)){

  }
  
};
#endif