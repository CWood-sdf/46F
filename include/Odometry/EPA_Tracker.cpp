#define NO_MAKE
#include "EPA_Tracker.h"
Positioner::Positioner(
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
    Encoder* enc = new Encoder(new encoder(port));
    EncodersX.push_back(enc);
    lastX.push_back(0);
    //Increase array access position
    i++;
  }

  i = 0;
  for(auto& port : yPorts){
    lastY.push_back(0);
    Encoder* enc = new Encoder(new encoder(port));
    //Allocate a completely new encoder from the heap and add it to the array
    EncodersY.push_back(enc);
    //Increase array access position
    i++;
  }
  encXAmnt = EncodersX.size();
  encYAmnt = EncodersY.size();
  cout << "Init Odom with " << encXAmnt << " x encoders and " << encYAmnt << " y encoders" << endl;
}
//Accepts port array and radius
Positioner::Positioner(
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
    Encoder* enc = new Encoder(new rotation(port.getPort()));
    EncodersX.push_back(enc);
    // AddDevice("Odom X Encoder "s + toCcp(i), (vex::device*)enc);
    lastX.push_back(0);
    //Increase array access position
    i++;
  }

  i = 0;
  for(auto& port : yPorts){
    lastY.push_back(0);
    Encoder* enc = new Encoder(new rotation(port.getPort()));
    //Allocate a completely new encoder from the heap and add it to the array
    EncodersY.push_back(enc);
    // AddDevice("Odom Y Encoder "s + toCcp(i), (vex::device*)enc);
    //Increase array access position
    i++;
  }
  encXAmnt = EncodersX.size();
  encYAmnt = EncodersY.size();
  cout << "Init Odom with " << encXAmnt << " x encoders and " << encYAmnt << " y encoders" << endl;
}
Positioner::Positioner(){
}
//Shifts angle to range of [0, 360)
double baseAngle(double ang){
  while(ang >= 360.0){
    ang -= 360.0;
  }
  while(ang < 0.0){
    ang += 360.0;
  }
  return ang;
}
//Shifts an angle to a range of (-180, 180]
double posNeg180(double ang){
  double ret = baseAngle(ang);
  while(ret > 180.0){
    ret -= 360.0;
  }
  while(ret <= -180.0){
    ret += 360.0;
  }
  return ret;
}
#ifndef WINDOWS
//Function that updates the position
//80+ lines of trig, vector math, and some sensor stuff
PVector Positioner::update(){
  //static double lostDist = 0.0;
  //static PVector last = PVector(1, 1);
  //Vector of the wheel angles
  PVector angles = PVector();

  //Get encoder rotation as fast as possible
  //Use raw array for speed
  vector<double> rotX;
  vector<double> rotY;
  //Update bot angle as close to rotation access as possible
  // updateBotAngle(run);
  if(EncodersX.size() != 0){
    for(int i = 0; i < EncodersX.size(); i++){
      //Don't do any multiplication (for speed)
      rotX.push_back(EncodersX[i]->position(rotationUnits::deg));
    }
  }
  // cout << EncodersY.size() << endl;
  if(EncodersY.size() != 0){
    for(int i = 0; i < EncodersY.size(); i++){
      rotY.push_back(EncodersY[i]->position(rotationUnits::deg));
      // cout << rotY[i] << endl;
      // s(1);
    }
  }

  for(int i = 0; i < EncodersX.size(); i++){
    //Get the rotation in radians
    double rot = posNeg180(rotX[i] - lastX[i]) * DEG_TO_RAD;// * multX[i];
    //double turnExpected = deltaBotAngle * DEG_TO_RAD * distFromCenterX[i];

    if(rot < 0){
      
      rot *= multNegX[i];
    } else if(rot > 0){
      rot *= multX[i];
    }
    //add the change in rotation to angles.x
    angles.x += rot;
    //Reset the last rotation
    lastX[i] = rotX[i];
    
  }

  //Average angles.x

  //Same thing here    
  for(int i = 0; i < EncodersY.size(); i++){
    double rot = posNeg180(rotY[i] - lastY[i]) * DEG_TO_RAD;// * multY[i];
    //double turnExpected = deltaBotAngle * DEG_TO_RAD * distFromCenterY[i];
    if(rot < 0){
      rot *= multNegY[i];
    } else if(rot > 0){
      rot *= multY[i];
    }
    angles.y += rot;
    lastY[i] = rotY[i];
  }
  if(EncodersY.size() != 0){
    angles.y /= encYAmnt;
  }
  if(EncodersX.size() != 0){
    angles.x /= encXAmnt;
  }
  angles *= wheelRad;
  PVector deltaAngles;
  
  
  if(deltaBotAngle != 0.0){
    double deltaAngle = deltaBotAngle * DEG_TO_RAD;
    double sin2 = 2.0 * sin(deltaAngle / 2.0);
    double x = (angles.x / deltaAngle + distFromCenterX) * sin2;
    double y = (angles.y / deltaAngle + distFromCenterY) * sin2;
    deltaAngles = { x, y };
    deltaAngles.rotate(avgBotAngle);
  } else {
    deltaAngles = angles;
    deltaAngles.rotate(avgBotAngle);
  }

  //Get the change in the position
  PVector deltaPos = deltaAngles;

  speed = deltaPos.dist2D() / (time.time(seconds));
  pos += deltaPos; // Add deltaPos to pos
  return pos; // Return pos so it can be used
}
PVector Positioner::getPos(){
  return PVector(-pos.x, -pos.y, 0.0);
}
double Positioner::xPosition(distanceUnits){
  return getPos().x;
}
double Positioner::yPosition(distanceUnits){
  return getPos().y;
}
double Positioner::heading(){
  return glblBotAngle;
}
FieldCoord Positioner::fullPos(){
  return FieldCoord(getPos(), heading());
}
bool Positioner::moving (){
  //If the velocity is greater than 0.01 in/s or 
  //its been less a second since the last call to clearMove
  return abs(speed) > 0.01 || 
          time.time(timeUnits::sec) < 1;
}
double Positioner::velocity(){
  return speed;
}
void Positioner::clearMove(){
  time.clear();
}
#endif