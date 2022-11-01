#ifndef GPS_SHARE_H
#define GPS_SHARE_H
#include "EPA_Tracker.h"
#ifndef TEST
class BasicConfirmationOdom {
public:
  virtual ~BasicConfirmationOdom() {}
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
  LinkedList<FieldCoord> gpsReadings;
  LinkedList<FieldCoord> odomReadings;
  vector<std::unique_ptr<BasicConfirmationOdom>> confirmOdoms; 
  FieldCoord pos = FieldCoord(PVector(0, 0), 0);
  FieldCoord lastOdom = FieldCoord(PVector(0, 0), 0);
  double speed = 0.0;
  bool lastBad = false;
  bool isFirstBad = false;
  static const int sleepTime = 10;
  static const int badTime = 200;
  //returns true if it's first call since reading gone bad
  //Resets when a good reading is obtained
  bool firstBad();
  bool readingBad();
public:
  GPS_Share(Positioner& o, gps& g);
  
  FieldCoord& fullPos();
  PVector& position();
  double heading();
  double velocity();
  bool gpsBad();
  void update();
  void setPos(PVector v, double a);
};
#endif
#endif