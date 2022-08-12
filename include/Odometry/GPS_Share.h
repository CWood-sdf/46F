#ifndef GPS_SHARE_H
#define GPS_SHARE_H
#include "Odometry/EPA_Tracker.h"
/**
A confirmation odometry system is any system that DOES NOT provide an angle
These systems depend on accurate angle readings to function properly
They also return the variance of their estimates
  - The variance is required for a confidence interval on the !gps odometry
**/
class BasicConfirmationOdom {
  virtual double xPosition(distanceUnits) = 0;
  virtual double yPosition(distanceUnits) = 0;
  virtual int32_t quality() = 0;
  virtual bool installed() = 0;
  virtual double variance() = 0;
  virtual void updateEstimate() = 0;
};
//Sleeps itself because it is top level odometry math
class GPS_Share {
  Positioner& odom;
  gps& GPS;
  vector<std::unique_ptr<BasicConfirmationOdom>> confirmOdoms; 
  LinkedList<Pose> gpsReadings;
  LinkedList<Pose> odomReadings;
  Pose pos;
  Pose lastOdom;
  double speed = 0.0;
  bool lastBad = false;
  bool isFirstBad = false;
  static const int sleepTime = 10;
  static const int badTime = 200;
  //returns true if it's first call since reading gone bad
  //Resets when a good reading is obtained
  bool firstBad(){
    if(isFirstBad){
      isFirstBad = false;
      return true;
    }
    return false;
  }
  bool readingBad(){
    Pose startPos = gpsReadings.getBase();
    //If the last time the readingBad() returned bad, the set the firstBad flag
    if(!lastBad){
      isFirstBad = true;
      lastBad = true;
    }
    else {
      lastBad = false;
    }
    for(auto p : gpsReadings){
      if(p != startPos){
        return false;
      }
    }
    return true;
  }
public:
  GPS_Share(Positioner& o, gps& g) : odom(o), GPS(g) {

  }
  
  Pose& fullPos(){
    return pos;
  }
  PVector& position(){
    return pos.pos;
  }
  double heading(){
    return pos.angle;
  }
  double velocity(){
    return speed;
  }
  bool gpsBad(){
    return GPS.quality() != 100 || !GPS.installed();
  }
  void update(){
    odom.update();

    //Get the odometry position
    Pose currentOdom = odom.fullPos();
    
    //Get the change in the odometry position
    Pose deltaOdom = currentOdom - lastOdom;
    //Reset the last odometry position for the next iteration
    lastOdom = currentOdom;
    //Get the speed
    //Uses odometry for speed because the GPS has random, slight variation
    //Odometry speed returns 0 when the speed is 0
    speed = deltaOdom.pos.mag() / (double)sleepTime * 1000.0;

    //Get GPS coordinate
    Pose gpsCoord = Pose(PVector(GPS.xPosition(inches), GPS.yPosition(inches)), GPS.heading());

    //If GPS can't see position
    if(gpsBad()){
      //Use change in odometry
      pos.pos += deltaOdom.pos;
      pos.angle += deltaOdom.angle;
    }
    else {
      //Set position to GPS value
      pos = gpsCoord;
    }
    //Sleep
    s(sleepTime);
  }
  void setPos(PVector v, double a){
    pos.pos = v;
    botAngles.x = a;
    pos.angle = 0;
    update();
  }
};
#endif