#include "Sensors/VisionOdom.h"
class Mogo {
  static GPS_Share* share;
  bool attachedBack = false;
  bool attachedFront = false;
  PVector estPos;
  //The place the robot would have to travel to to go to a mogo
  PVector closest(double dist, bool back){
    PVector botPos = share->position();
    PVector pos = estPos;
    double angle = pos.angleTo(botPos);
    PVector half = PVector(dist, 0);
    half.rotate(angle - half.heading2D()).add(pos);
    return half;
  }
  //The place the robot would have to travel to to go to a mogo
  PVector setNew(double dist, bool back){
    PVector botPos = share->position();
    double angle = share->heading() + 180.0 * back;
    PVector newPos = PVector(dist, 0);
    newPos.rotate(angle - newPos.heading2D()).add(botPos);
    estPos = newPos;
    return newPos;
  }
public:
  Mogo(PVector est){
    estPos = est;
  }
  Mogo(GPS_Share& g, PVector est) : Mogo(est){
    init(g);
  }
  bool isAttachedBack(){
    return attachedBack;
  }
  bool isAttachedFront(){
    return attachedFront;
  }
  void attachFront(){
    attachedFront = true;
  }
  void detachBack(){
    setNew(21, true);
    attachedBack = false;
  }
  void attachBack(){
    attachedBack = true;
  }
  void detachFront(){
    setNew(21, false);
    attachedFront = false;
  }
  PVector closestBack(){
    
    return closest(14, true);
  }
  PVector closestFront(){ // Same as closestBack() for now, but dist might change
    return closest(14, false);
  }
  
  static void init(GPS_Share& g){
    Mogo::share = &g;
  }
};