#define NO_MAKE
#include "IntakeAutomation.h"
AutoIntake::AutoIntake(vector<LineCounter*> sensors){
  this->sensors = sensors;
}
void AutoIntake::makeMask(){
  diskMask = 0;
  for(int i = 0; i < sensors.size(); i++){
    diskMask |= (sensors[i]->pressing() << i);
  }
}
bool AutoIntake::stable(){
  //Is stable if it does not end with 0's
  // (i.e. return true if 0b011 or 0b001 or 0b111 or 0b000 but not 0b010 or 0b100)
  int lastBit = 1;
  for(int i = 0; i < sensors.size(); i++){
    int bit = (diskMask >> i) & 1;
    if(bit == 1 && lastBit == 0){
      return false;
    }
  }
  return true;
}
bool AutoIntake::fixableUnstable(){
  //Is fixable if it ends with 0's
  // (i.e. return true if 0b010 or 0b100 but not 0b011 or 0b001 or 0b111 or 0b000 or 0b101)
  int lastBit = 0;
  for(int i = 0; i < sensors.size(); i++){
    int bit = (diskMask >> i) & 1;
    if(bit == 0 && lastBit == 1){
      return false;
    }
    lastBit = bit;
  }
  return true;
}

void AutoIntake::disable(){
  disabled = true;
}

void AutoIntake::enable(){
  disabled = false;
}

void AutoIntake::updateValues(bool flywheelReady){
  makeMask();
  
//   for(int i = 0; i < sensors.size(); i++){
//     ((bool*)&disk)[i] = (diskMask >> i) & 1;
//   }
  count = 0;
  for(int i = 0; i < sensors.size(); i++){
    count += sensors[i]->pressing();
  }
  if(clearingDisks && count == 0){
    clearingDisks = false;
  }
  if(intaking && sensors.back()->pressing()){
    intaking = false;
  }
  if(intaking && count == lastCount + 1){
    intaking = false;
  }
  if(!intaking){
    lastCount = count;
  }
  //If seconds of clearing disks, then stop
  if(clearingDisks && (Brain.Timer.system() - clearStartTime) > 5000){
    clearingDisks = false;
  }
  direction = 0;
  if(intaking || (clearingDisks && flywheelReady)){
    direction = 1;
  }
  if(direction != 1 && ((!stable() && fixableUnstable()) || fixingUnstable)){
    direction = -1;
    fixingUnstable = true;
  }
  if(stable()){
    fixingUnstable = false;
  }
}

bool AutoIntake::hasDisks(){
  return count > 0;
}

void AutoIntake::setFiring(){
  clearingDisks = true;
  clearStartTime = Brain.Timer.system();
}

void AutoIntake::intake(){
  intaking = true;
}

bool AutoIntake::spinMotor(){
  return direction == 1;
}

bool AutoIntake::reverseMotor(){
  return direction == -1;
}
