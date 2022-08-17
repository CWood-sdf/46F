#ifndef GPS_SHARE_H
#define GPS_SHARE_H
#include "Odometry/EPA_Tracker.h"
//Sleeps itself because it is top level odometry math
class GPS_Share {
  Positioner& odom;
  gps& GPS;
  LinkedList<FieldCoord> gpsReadings;
  LinkedList<FieldCoord> odomReadings;
  FieldCoord pos = FieldCoord(PVector(0, 0), 0);
  FieldCoord lastOdom = FieldCoord(PVector(0, 0), 0);
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
    FieldCoord startPos = gpsReadings.getBase();
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
  
  FieldCoord& fullPos(){
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
    FieldCoord currentOdom = odom.fullPos();
    
    //Get the change in the odometry position
    FieldCoord deltaOdom = currentOdom - lastOdom;
    //Reset the last odometry position for the next iteration
    lastOdom = currentOdom;
    //Get the speed
    //Uses odometry for speed because the GPS has random, slight variation
    //Odometry speed returns 0 when the speed is 0
    speed = deltaOdom.pos.mag() / (double)sleepTime * 1000.0;

    //Get GPS coordinate
    FieldCoord gpsCoord = FieldCoord(PVector(GPS.xPosition(inches), GPS.yPosition(inches)), GPS.heading());

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