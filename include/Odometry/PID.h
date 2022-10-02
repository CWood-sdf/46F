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
  PidfAdder(double p, double i, double d, double f = 0.0);
  PidfAdder();
};

struct PIDF_Extension {
  virtual ~PIDF_Extension() {}
  PIDF_Extension() {

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
    double p = 0.0, i = 0.0, d = 0.0, f = 0.0;
    KVals();
    //Set the values
    KVals& operator=(KVals& vals);
    //Constructor with initializer_list
    //KVals({0.1, 0.2, 0.3});
    KVals(std::initializer_list<double> vals);
    //Add a similar KVal class (Like PidAdder) to the values
    template<class T>
    KVals& operator+=(T a);
    //Subtract
    template<class T>
    KVals& operator-=(T a);
    
  };
  //Stores the multiplication values under k
  KVals k;

  //Variables
  double target = 0.0, error = 0.0, lastError = 0.0, iCap = 0.0, iGrowth = 0.0, iZero = 0.0;
public:
  std::shared_ptr<PIDF_Extension> manager = std::shared_ptr<PIDF_Extension>(NULL);
  PIDF();
  PIDF(KVals vals, PIDF_Extension& mgr, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0);
  PIDF(double p, double i, double d, PIDF_Extension& mgr, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0);
  PIDF(double p, double i, double d, double f, PIDF_Extension& mgr, double iCap = 0.0, double iGrowthRange = 0.0, double iZeroRange = 0.0);
  //Constructors
  PIDF(KVals vals, double iCap, double iGrowthRange, double iZeroRange);
  PIDF(double p, double i, double d, double iCap, double iGrowthRange, double iZeroRange);
  PIDF(double p, double i, double d, double f, double iCap, double iGrowthRange, double iZeroRange);
  PIDF(double p, double i, double d, double f);
  PIDF(double p, double i, double d);
  PIDF(KVals vals);
  PIDF(const PIDF& v);
  PIDF(PIDF&& v);
  //Get the error
  double getError();
  //Clear out the previous PID usage
  void resetVals();
  //Set the target value of the PID
  void setTarget(double val);
  //Apply the error
  void incVals(double sensorVal);
  //Get the speed value given that error has already been applied
  double getVal();
  //Apply error, then return getVal()
  double getVal(double sensorVal);
  //Add a PidAdder
  PIDF& operator+= (PidfAdder a);
  //Subtract a PidAdder
  PIDF& operator-= (PidfAdder a);
  PIDF& operator=(PIDF&& a);
  //Directly taken from the copy constructor
  PIDF& operator=(const PIDF& a);
  KVals getKVals();
};

typedef PIDF PID;
typedef PidfAdder PidAdder;
typedef PIDF_Extension PID_Extension;

ostream& operator<<(ostream& cout, PIDF&& v);
ostream& operator<<(ostream& cout, PIDF& p);
#endif