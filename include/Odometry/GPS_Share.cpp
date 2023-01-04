#define NO_MAKE
#include "GPS_Share.h"
#ifndef WINDOWS
bool GPS_Share::firstBad()
{
  if (isFirstBad)
  {
    isFirstBad = false;
    return true;
  }
  return false;
}
bool GPS_Share::readingBad()
{
  FieldCoord startPos = gpsReadings.getBase();
  // If the last time the readingBad() returned bad, the set the firstBad flag
  if (!lastBad)
  {
    isFirstBad = true;
    lastBad = true;
  }
  else
  {
    lastBad = false;
  }
  for (auto p : gpsReadings)
  {
    if (p != startPos)
    {
      return false;
    }
  }
  return true;
}
GPS_Share::GPS_Share(Positioner &o, gps &g) : odom(o), GPS(g), gpsEma(0.7, FieldCoord(PVector(0, 0), 0))
{
}

FieldCoord &GPS_Share::fullPos()
{
  return pos;
}
PVector &GPS_Share::position()
{
  return pos.pos;
}
double GPS_Share::heading()
{
  return pos.angle;
}
double GPS_Share::velocity()
{
  return speed;
}
bool GPS_Share::gpsBad()
{
  return GPS.quality() != 100 || !GPS.installed();
}
void GPS_Share::update()
{
  static FieldCoord lastPosReading = FieldCoord(PVector(0, 0), 0);
  odom.update();

  // Get the odometry position
  FieldCoord currentOdom = odom.fullPos();

  // Get the change in the odometry position
  FieldCoord deltaOdom = currentOdom - lastOdom;
  // Reset the last odometry position for the next iteration
  lastOdom = currentOdom;
  // Get the speed
  // Uses odometry for speed because the GPS has random, slight variation
  // Odometry speed returns 0 when the speed is 0
  speedFilter.update(deltaOdom.pos.mag() / (double)sleepTime * 1000.0);
  speed = speedFilter.value();
  // Get GPS coordinate
  FieldCoord gpsCoord = FieldCoord(PVector(GPS.xPosition(inches), GPS.yPosition(inches)), GPS.heading());

  // If GPS can't see position
  if (gpsBad())
  {
    // Use change in odometry
    pos.pos += deltaOdom.pos;
    pos.angle += deltaOdom.angle;
  }
  else
  {
    // Set position to GPS value
    pos = gpsCoord;
  }
  if (!gpsBad())
  {
    // Avg last 2
    pos.pos = (pos.pos + lastPosReading.pos) / 2.0;
    pos.angle = (pos.angle + lastPosReading.angle) / 2.0;
    gpsEma.update(pos);
    pos = gpsEma.value();
  }
  lastPosReading = pos;
  // Sleep
  s(sleepTime);
}
void GPS_Share::setPos(PVector v, double a)
{
  pos.pos = v;
  // botAngles.x = a;
  pos.angle = a;
  update();
}
#endif