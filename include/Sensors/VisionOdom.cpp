#define NO_MAKE
#include "Sensors/VisionOdom.h"
Angle operator "" _deg(long double v){
    return Angle((double) v, Angle::Type::Deg);
}
Angle operator "" _rev(long double v){
    return Angle((double) v, Angle::Type::Rev);
}
Angle operator "" _rad(long double v){
    return Angle((double) v, Angle::Type::Rad);
}