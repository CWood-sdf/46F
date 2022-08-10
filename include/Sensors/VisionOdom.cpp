#define NO_MAKE
#include "Sensors/VisionOdom.h"
Angle operator "" _rad(long double value){
  return Angle(value, Angle::Type::Rad);
}
Angle operator "" _deg(long double value){
  return Angle(value, Angle::Type::Deg);
}