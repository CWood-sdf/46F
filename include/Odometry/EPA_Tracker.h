#ifndef EPA_TRACKER_H
#define EPA_TRACKER_H
#include "FieldCoord.h"
#include "Sensors/Wrappers/Encoder.h"
//EPA_Tracker.h -- Use this file to track the robot's absolute position on the field
//This file does so much math that it be like Beethoven 9 if it all works properly

//Make some basic conversion numbers
//To use, just multiply the number by the conversion
#define CM_TO_IN (1.0 / 2.54)
#define IN_TO_CM 2.54
#define DEG_TO_RAD (M_PI / 180.0)
#define RAD_TO_DEG (1.0 / DEG_TO_RAD)
typedef unsigned int uint;

double baseAngle(double);
double posNeg180(double);
void updateBotAngle(bool add = false);
//A class that stores a reference of something as a pointer
//  this is used so that I can put references in arrays
template<class tp>
struct Ref {
  tp* val;
  Ref(){}
  Ref(tp& v){
    val = &v;
  }
  tp& operator*(){
    return *val;
  }
  operator tp& (){
    return *val;
  }
  operator tp*(){
    return val;
  }
  tp* operator->(){
    return val;
  }
};
// //Use the template command for the amount of encoders used
// //Each encoder is used for each dimension
// //Positioner<2, 2> has four encoders total 2x, 2y
// template<uint encodersX, uint encodersY>
class Port {
  const int32_t port;
  public:
  Port(int32_t p): port(p){}
  int32_t getPort(){
    return port;
  }
};
#ifndef WINDOWS
class Positioner {
//A few typedefs
public:
  typedef vector<Encoder*> yEncoderArr; // Defines the type of the arrays
      //That the encoders will be stored in and names it encoderArr
  typedef vector<Encoder*> xEncoderArr;
  typedef vector<Ref<vex::triport::port>> xTriportArr; // Defines the type of the arrays
      //That the encoders will be stored in and names it encoderArr
  typedef vector<Ref<vex::triport::port>> yTriportArr;
  typedef vector<Port> xPortArr;
  typedef vector<Port> yPortArr;
  //const double size = encodersX + encodersY;
//Private variables
private:

  double encXAmnt;// = (double) encodersX;
  double encYAmnt;// = (double) encodersY;
  typedef vector<double> xDoubleArr;
  typedef vector<double> yDoubleArr;
  xDoubleArr lastX;
  yDoubleArr lastY;
  xDoubleArr multX;
  yDoubleArr multY;
  xDoubleArr multNegX;
  yDoubleArr multNegY;
  double distFromCenterX;
  double distFromCenterY;

  timer time;

  xEncoderArr EncodersX; // Make the x encoder array
  yEncoderArr EncodersY;
  double lastLeft = 0.0, lastRight = 0.0;
  friend void waitForReset();
  void resetPos(PVector pos){
    this->pos = pos;
  }
public:
private:
  //array<double, encodersY> lastY;
  //array<double, encodersX> lastX;
  //PVector lastAngles = PVector(0.0, 0.0); // Make a vector that stores the last angles
  PVector pos = PVector(0.0, 0.0); // Make a vector to store the current position
  double wheelRad = 0.0; // A variable that stores the wheel radius in inches for
          // distance calculations later
  //Useless variables that I'm keeping just in case they become useful eventually
  //double updates_p_second = 500;
  //double& ups = updates_p_second;
  double speed = 0.0;
  //The main functions: constructor, updater...
  Positioner(xDoubleArr mX, yDoubleArr mY, xDoubleArr mNX, yDoubleArr mNY, double cDistX, double cDistY, double rad){
    wheelRad = rad;
    multX = mX;
    multY = mY;
    multNegX = mNX;
    multNegY = mNY;
    distFromCenterX = cDistX;
    distFromCenterY = cDistY;
  }
public:
  //The constructors

  //Accepts port array and radius
  Positioner(
    xTriportArr xPorts, yTriportArr yPorts, 
    xDoubleArr mX, yDoubleArr mY, 
    xDoubleArr mNX, yDoubleArr mNY, 
    double cDistX, double cDistY, 
    double rad
  ) : Positioner(mX, mY, mNX, mNY, cDistX, cDistY, rad)
  {
    int i = 0;
    //I know, I know, using 'new' is bad but, like, I mean, 
    // it's used only in the global scope, so all memory is deallocated by OS, not me
    for(auto& port : xPorts){
      //Allocate a completely new encoder from the heap and add it to the array
      EncodersX.push_back(new Encoder(new encoder(port)));
      lastX.push_back(0);
      //Increase array access position
      i++;
    }

    i = 0;
    for(auto& port : yPorts){
      lastY.push_back(0);
      //Allocate a completely new encoder from the heap and add it to the array
      EncodersY.push_back(new Encoder(new encoder(port)));
      //Increase array access position
      i++;
    }
    encXAmnt = EncodersX.size();
    encYAmnt = EncodersY.size();
    cout << "Init Odom with " << encXAmnt << " x encoders and " << encYAmnt << " y encoders" << endl;
  }
  //Accepts port array and radius
  Positioner(
    xPortArr xPorts, yPortArr yPorts, 
    xDoubleArr mX, yDoubleArr mY, 
    xDoubleArr mNX, yDoubleArr mNY, 
    double cDistX, double cDistY, 
    double rad
  ) : Positioner(mX, mY, mNX, mNY, cDistX, cDistY, rad)
  {
    int i = 0;
    //I know, I know, using 'new' is bad but, like, I mean, 
    // it's used only in the global scope, so all memory is deallocated by OS, not me
    for(auto& port : xPorts){
      //Allocate a completely new encoder from the heap and add it to the array
      EncodersX.push_back(new Encoder(new rotation(port.getPort())));
      lastX.push_back(0);
      //Increase array access position
      i++;
    }

    i = 0;
    for(auto& port : yPorts){
      lastY.push_back(0);
      //Allocate a completely new encoder from the heap and add it to the array
      EncodersY.push_back(new Encoder(new rotation(port.getPort())));
      //Increase array access position
      i++;
    }
    encXAmnt = EncodersX.size();
    encYAmnt = EncodersY.size();
    cout << "Init Odom with " << encXAmnt << " x encoders and " << encYAmnt << " y encoders" << endl;
  }
  Positioner(){
  }
  //Function that updates the position
  //80+ lines of trig, vector math, and some sensor stuff
  PVector update();
  PVector getPos();
  double xPosition(distanceUnits=inches);
  double yPosition(distanceUnits=inches);
  double heading();
  FieldCoord fullPos();
  bool moving ();
  double velocity();
  void clearMove();
};
#endif
#endif