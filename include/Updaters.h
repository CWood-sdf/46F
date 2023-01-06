#include "ClassBaseFns.h"
#ifdef NO_MAKE
void updateBotAngle(bool);
void microWait(uint);
void executeThreads();
void updateSharePos();
#else

// Update the bot angle in all three dimensions
void updateBotAngle(bool add)
{
  //
  // bool gpsFirst = false;
  static double lastAngle = angler.orientation(orientationType::yaw, rotationUnits::deg);

  static double lastAngleYZ = angler.orientation(orientationType::pitch, rotationUnits::deg);
  static double lastAngleXZ = angler.orientation(orientationType::roll, rotationUnits::deg);
  //
  double err = 360.0 / 359.5;
  double errNeg = 360.0 / 359.5;

  double errYZ = 1.0;
  double errXZ = 1.0;
  static int i = 0;
  //
  // if(!share.gpsBad()/* && !gpsFirst*/){
  //   // cout << "Share not bad: " << share.heading() << endl;
  //   botAngles.x = posNeg180(share.heading());
  //   // gpsFirst = true;
  // }
  double angle = angler.orientation(orientationType::yaw, rotationUnits::deg);
  // cout << angle << endl;
  double gain = posNeg180(angle - lastAngle);
  gain *= (gain > 0 ? err : errNeg);
  avgBotAngle = botAngles.x + gain / 2.0;
  botAngles.x += gain;
  deltaBotAngle = gain;
  lastAngle = angle;

  angle = angler.orientation(orientationType::pitch, rotationUnits::deg);
  gain = posNeg180(angle - lastAngleYZ);
  botAngles.y += gain * errYZ;
  lastAngleYZ = angle;

  angle = angler.orientation(orientationType::roll, rotationUnits::deg);
  gain = posNeg180(angle - lastAngleXZ);
  botAngles.z += gain * errXZ;
  lastAngleXZ = angle;

  tiltAngle = botAngles.z - startTilt;
  // Start off with 1 in y to mimic field setup
  //  PVector angleObtainer = PVector(0.0, 10.0, 0.0);
  //  //Rotate by basic field angle

  // angleObtainer.rotateXY(botAngles.x + 90.0);
  // angleObtainer.rotateYZ(botAngles.y);
  // angleObtainer.rotateXZ(botAngles.z);
  // tiltAngle = angleObtainer.headingYZ();
  if (i++ == 20)
  {
    // cout << glblBotAngle << endl;
    i = 0;
  }
}

void microWait(uint time)
{
  auto startTime = Brain.Timer.systemHighResolution();
  int i = 0;
  while (Brain.Timer.systemHighResolution() - startTime < time || i < -10)
  {
    i++;
  }
}

void executeThreads()
{

  // Execute autonomous functions
  while (1)
  {
    if (threadFns.size() > 0)
    {
      threadFns.front()();
      threadFns.pop_front();
    }
    else
    {
      this_thread::sleep_for(100);
    }
  }
}
void updateSharePos()
{
  while (1)
  {
    updateBotAngle();
    share.update();
  }
}
#endif