#pragma once
namespace vex {
enum class gearSetting {
	ratio18_1,
	ratio36_1,
	ratio6_1
};
enum class directionType {
	forward,
	reverse
};
enum class timeUnits {
	msec
};
}
using namespace vex;
extern const directionType& reverse = directionType::reverse;
extern const directionType& fwd = directionType::forward;
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <functional>
#include <thread>
#include <cmath>
#include <map>
using namespace std;
#ifdef NO_MAKE
#define TEMP
#endif
#undef NO_MAKE
#include "C:/Users/woodc/OneDrive/GitHub/Libs-Actual/Libs/PVector.h"
#include "C:/Users/woodc/OneDrive/GitHub/Libs-Actual/Libs/Matrix.h"
ostream& operator<< (ostream& cout, PVector& v){
  cout << VECT_OUT(v);
  return cout;
}
ostream& operator<<(ostream& cout, PVector&& v){
  return operator<<(cout, v);
}
#ifdef TEMP
#define NO_MAKE
#undef TEMP
#endif
void s(int t){
	std::this_thread::sleep_for(std::chrono::milliseconds(t));
}