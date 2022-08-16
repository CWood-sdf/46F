#ifndef PID_H
#define PID_H
#include "vex.h"

//PID.h -- use this file for a PID class
class PIDF;
//Stores numbers to add to a PID's KVals
class PidfAdder {
  friend class PIDF;
  double p, i, d, f;
public:
  //Constructor
  //PidAdder(0.1, 0.2, 0.1);
  PidfAdder(double p, double i, double d, double f = 0.0){
    this->p = p;
    this->i = i;
    this->d = d;
    this->f = f;
  }
  PidfAdder(){
    
  }
};

struct PIDF_Extension {
  PIDF_Extension(){

  }
  virtual void manageP(double& p){

  }
  virtual void manageI(double& i){

  }
  virtual void manageD(double& d){

  }
  virtual double getVal(double ret){
    return ret;
  }
  virtual PIDF_Extension* getCopy(){
    return new PIDF_Extension(*this);
  }
  virtual double manageInput(double in){
    return in;
  }
};
//Some nice simple math
/********************************
.                     ____   ___    ______
ENSURE THAT THERE ARE |   \  | |   |  __  |
.                     | |\ \ | |   | |  | |
.                     | | \ \| |   | |__| |
.                     |_|  \___|   |______|
NON-DOUBLE TYPE VARIABLES BEFORE THE DOUBLE TYPE VARIABLES 
    (This is for hacky pointer copy constructor)
********************************/
class PIDF {
  public:
  //The values to multiply the K values by
  double p = 0.0, i = 0.0, d = 0.0, f = 0.0;
  //A simple struct that stores the multiplication values
  struct KVals {
    //The variables
    double p, i, d, f = 0.0;
    KVals(){
      p = i = d = f = 0.0;
    }
    //Set the values
    KVals& operator=(KVals& vals){
      p = vals.p;
      i = vals.i;
      d = vals.d;
      f = vals.f;
      return *this;
    }
    //Constructor with initializer_list
    //KVals({0.1, 0.2, 0.3});
    KVals(std::initializer_list<double> vals){
      double* ths = (double*) this;
      for(double val : vals){
        *ths = val;
        ++ths;
      }

    }
    //Add a similar KVal class (Like PidAdder) to the values
    template<class T>
    KVals& operator+=(T a){
      p += a.p;
      i += a.i;
      d += a.d;
      f += a.f;
      return *this;
    }
    //Subtract
    template<class T>
    KVals& operator-=(T a){
      p -= a.p;
      i -= a.i;
      d -= a.d;
      f -= a.f;
      return *this;
    }
    
  };
  //Stores the multiplication values under k
  KVals k;

  //Variables
  double target = 0.0, error = 0.0, lastError = 0.0, iCap = 0.0, iGrowth = 0.0, iZero = 0.0;
public:
  std::shared_ptr<PIDF_Extension> manager = std::shared_ptr<PIDF_Extension>();
  PIDF(){

  }
  PIDF(KVals vals, PIDF_Extension& mgr, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0) : PIDF(vals, iCap, iGrowthRange, iZeroRange){
    manager.reset(&mgr);
  }
  PIDF(double p, double i, double d, PIDF_Extension& mgr, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0) : PIDF(p, i, d, 0.0, iCap, iGrowthRange, iZeroRange){
    manager.reset(&mgr);
  }
  PIDF(double p, double i, double d, double f, PIDF_Extension& mgr, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0) : PIDF(p, i, d, f, iCap, iGrowthRange, iZeroRange){
    manager.reset(&mgr);
  }
  //Constructors
  PIDF(KVals vals, double iCap, double iGrowthRange, double iZeroRange) : k(vals){
    this->iCap = iCap;
    iGrowth = iGrowthRange;
    iZero = iZeroRange;
  }
  PIDF(double p, double i, double d, double iCap, double iGrowthRange, double iZeroRange) : PIDF(KVals({ p, i, d, 0.0 })) {
    this->iCap = iCap;
    iGrowth = iGrowthRange;
    iZero = iZeroRange;
  }
  PIDF(double p, double i, double d, double f, double iCap, double iGrowthRange, double iZeroRange) : PIDF(KVals({ p, i, d, f })) {
    this->iCap = iCap;
    iGrowth = iGrowthRange;
    iZero = iZeroRange;
  }
  PIDF(double p, double i, double d, double f) : PIDF(KVals({p, i, d, f})){}
  PIDF(double p, double i, double d) : PIDF(KVals({p, i, d, 0.0})){}
  PIDF(KVals vals) : k(vals){

  }
  PIDF(const PIDF& v){
    //i hate this but it should work
    double* doubles = (double*)&v;
    double* thisPtr = (double*)this;
    int size = (sizeof(PIDF) - sizeof(std::shared_ptr<PIDF_Extension>)) / sizeof(double);
    for(int i = 0; i < size; i++){
      thisPtr[i] = doubles[i];
    }
    resetVals();
    manager.reset(v.manager->getCopy());
  }
  PIDF(PIDF&& v) {
    *this = std::move(v);
  }
  //Get the error
  double getError(){
    return error;
  }
  //Clear out the previous PID usage
  void resetVals(){
    error = 0.0;
    p = i = d = 0.0;
  }
  //Set the target value of the PID
  void setTarget(double val){
    resetVals();
    target = val;
  }
  //Apply the error
  void incVals(double sensorVal){
    
    lastError = error;
    error = manager->manageInput(target - sensorVal);
    p = error;
    if(abs(error) <= iGrowth && iGrowth != 0.0){
      i += error;
    }
    if(abs(i) > abs(iCap) && abs(iCap) > 1.0){
      i = iCap;
    }
    if(abs(error) <= iZero && iZero != 0.0){
      i = 0.0;
    }
    d = error - lastError;
    manager->manageP(p);
    manager->manageI(i);
    manager->manageD(d);
  }
  //Get the speed value given that error has already been applied
  double getVal(){
    double pInc = k.p * p;
    double iInc = k.i * this->i;
    double dInc = k.d * this->d;
    return manager->getVal(pInc + iInc + dInc);
  }
  //Apply error, then return getVal()
  double getVal(double sensorVal){
    
    this->incVals(sensorVal);
    return getVal();
  }
  //Add a PidAdder
  PIDF& operator+= (PidfAdder a){
    k += a;
    return *this;
  }
  //Subtract a PidAdder
  PIDF& operator-= (PidfAdder a){
    k -= a;
    return *this;
  }
  PIDF& operator=(PIDF&& a){
    return operator=(a);
  }
  //Directly taken from the copy constructor
  PIDF& operator=(const PIDF& a){
    //i hate this but it should work
    double* doubles = (double*)&a;
    double* thisPtr = (double*)this;
    int size = (sizeof(PIDF) - sizeof(std::shared_ptr<PIDF_Extension>)) / sizeof(double);
    for(int i = 0; i < size; i++){
      thisPtr[i] = doubles[i];
    }

    //Clear any error build up
    resetVals();
    manager.reset(a.manager->getCopy());
    CHAIN
  }
  KVals getKVals(){
    return k;
  }
};

typedef PIDF PID;
typedef PidfAdder PidAdder;
typedef PIDF_Extension PID_Extension;

#ifndef NO_MAKE
ostream& operator<<(ostream& cout, PIDF&& v){
  cout << v.getKVals().p << ", " << v.getKVals().i << ", " << v.getKVals().d << ", " << v.getKVals().f << endl;
  return cout;
}
ostream& operator<<(ostream& cout, PIDF& p){
  cout << p.k.p << ", " << p.k.i << ", " << p.k.d << ", " << p.k.f << endl;
  return cout;
}
#else 
ostream& operator<<(ostream& cout, PIDF&& v);
ostream& operator<<(ostream& cout, PIDF& p);
#endif
#endif
