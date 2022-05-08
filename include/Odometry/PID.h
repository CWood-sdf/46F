#include "vex.h"
//PID.h -- use this file for a PID class
class PID;
//Stores numbers to add to a PID's KVals
class PidAdder {
  friend class PID;
  double p, i, d;
public:
  //Constructor
  //PidAdder(0.1, 0.2, 0.1);
  PidAdder(double p, double i, double d){
    this->p = p;
    this->i = i;
    this->d = d;
  }
  PidAdder(){
    
  }
};

class PID_Extension {
  virtual double newD(double newVal){
    return newVal;
  }
  virtual double newP(double newVal){
    return newVal;
  }
  virtual double addToI(double newVal){
    return newVal;
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
};
//Some nice simple math
class PID {
  public:
  //The values to multiply the K values by
  double p = 0.0, i = 0.0, d = 0.0;
  //A simple struct that stores the multiplication values
  struct KVals {
    //The variables
    double p, i, d;
    KVals(){
      p = i = d = 0.0;
    }
    //Set the values
    KVals& operator=(KVals& vals){
      p = vals.p;
      i = vals.i;
      d = vals.d;
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
      return *this;
    }
    //Subtract
    template<class T>
    KVals& operator-=(T a){
      p -= a.p;
      i -= a.i;
      d -= a.d;
      return *this;
    }
    
  };
  //Stores the multiplication values under k
  KVals k;

  //Variables
  double target = 0.0, error = 0.0, lastError = 0.0, iCap, iGrowth, iZero;
public:
  PID(){

  }
  //Constructors
  PID(KVals vals, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0) : k(vals){
    this->iCap = iCap;
    iGrowth = iGrowthRange;
    iZero = iZeroRange;
  }
  PID(double p, double i, double d, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0) : PID(KVals({ p, i, d })) {
    this->iCap = iCap;
    iGrowth = iGrowthRange;
    iZero = iZeroRange;
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
    error = target - sensorVal;
    p = error;
    if(abs(error) <= iGrowth && iGrowth != 0.0){
      i += error;
    }
    if(i > iCap && iCap != 0.0){
      i = iCap;
    }
    if(abs(error) <= iZero && iZero != 0.0){
      i = 0.0;
    }
    d = error - lastError;
  }
  //Get the speed value given that error has already been applied
  double getVal(){
    double pInc = k.p * p;
    double iInc = k.i * this->i;
    double dInc = k.d * this->d;
    return pInc + iInc + dInc;
  }
  //Apply error, then return getVal()
  double getVal(double sensorVal){
    
    this->incVals(sensorVal);
    return getVal();
  }
  //Add a PidAdder
  PID& operator+= (PidAdder a){
    k += a;
    return *this;
  }
  //Subtract a PidAdder
  PID& operator-= (PidAdder a){
    k -= a;
    return *this;
  }
  KVals getKVals(){
    return k;
  }
};




class PIDF;
//Stores numbers to add to a PID's KVals
class PidfAdder {
  friend class PIDF;
  double p, i, d, f;
public:
  //Constructor
  //PidAdder(0.1, 0.2, 0.1);
  PidfAdder(double p, double i, double d, double f){
    this->p = p;
    this->i = i;
    this->d = d;
    this->f = f;
  }
  PidfAdder(){
    
  }
};
class PIDF {
  public:
  //The values to multiply the K values by
  double p = 0.0, i = 0.0, d = 0.0, f = 0.0;
  //A simple struct that stores the multiplication values
  struct KVals {
    //The variables
    double p, i, d, f;
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
  double target = 0.0, error = 0.0, lastError = 0.0, iCap, iGrowth, iZero;
public:
  PIDF(){

  }
  //Constructors
  PIDF(KVals vals, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0) : k(vals){
    this->iCap = iCap;
    iGrowth = iGrowthRange;
    iZero = iZeroRange;
  }
  PIDF(double p, double i, double d, double f, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0) : PIDF(KVals({ p, i, d, f })) {
    this->iCap = iCap;
    iGrowth = iGrowthRange;
    iZero = iZeroRange;
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
    error = target - sensorVal;
    p = error;
    if(abs(error) <= iGrowth && iGrowth != 0.0){
      i += error;
    }
    if(i > iCap && iCap != 0.0){
      i = iCap;
    }
    if(abs(error) <= iZero && iZero != 0.0){
      i = 0.0;
    }
    d = error - lastError;
  }
  //Get the speed value given that error has already been applied
  double getVal(){
    double pInc = k.p * p;
    double iInc = k.i * this->i;
    double dInc = k.d * this->d;
    return pInc + iInc + dInc;
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
  KVals getKVals(){
    return k;
  }
};
#ifndef NO_MAKE
ostream& operator<<(ostream& cout, PID&& v){
  cout << v.getKVals().p << ", " << v.getKVals().i << ", " << v.getKVals().d << endl;
  return cout;
}
ostream& operator<<(ostream& cout, PID& p){
  cout << p.k.p << ", " << p.k.i << ", " << p.k.d << endl;
  return cout;
}
ostream& operator<<(ostream& cout, PIDF&& v){
  cout << v.getKVals().p << ", " << v.getKVals().i << ", " << v.getKVals().d << ", " << v.getKVals().f << endl;
  return cout;
}
ostream& operator<<(ostream& cout, PIDF& p){
  cout << p.k.p << ", " << p.k.i << ", " << p.k.d << ", " << p.k.f << endl;
  return cout;
}
#else 
ostream& operator<<(ostream& cout, PID&& v);
ostream& operator<<(ostream& cout, PID& p);
ostream& operator<<(ostream& cout, PIDF&& v);
ostream& operator<<(ostream& cout, PIDF& p);
#endif