//#include "EPA_Tracker.h"
#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H
#include "Sensors/PotDial.h"
#include "Sensors/LineCounter.h"
// #include "Sensors/Distance.h"
#include "Sensors/VisionOdom.h"
#include "Odometry/EPA_Wheel_Control.h"
#include "Flywheel/Controller.h"
#include "Sensors/Wrappers/Encoder.h"
/*************************************

Motors

*************************************/
//Front Left Wheel (FL)
extern motor FL;
//Front Right Wheel (FR)
extern motor FR;
//Back Left Wheel (BL)
extern motor BL;
//Back Right Wheel (BR)
extern motor BR;
//Middle Left Wheel (ML)
extern motor ML;
//Middle Right Wheel (MR)
extern motor MR;

extern Pto leftPto;
extern Pto rightPto;

extern motor_group Left;
extern motor_group Right;

extern motor flyWheelMot;
extern motor flywheel2;

extern NewMotor intake;

extern NewMotor flywheelNm;
// extern FlywheelPID flyPID;
extern FlywheelTBHEncoder flyTBH;
// extern bool flywheelPID;
//New Motors, a few reasons for this: 
//    1 - less upfront code for stuff
//    2 - Simplified spin cmd
extern NewMotor wheels;
extern NewMotor leftWheels;
extern NewMotor rightWheels;

extern Chassis chassis;
extern PurePursuitController purePursuit;
extern RamseteController ramsete;
extern BasicPidController pidController;

/*************************************

Pneumatics

*************************************/
extern pneumatics endgame;
extern pneumatics pto;

/*************************************

Sensors

*************************************/


//Three wire expander
extern triport Expander;

//Inertial Sensor
extern inertial angler;

//gps
extern gps GPS;

extern optical rachetColor; 

extern LineCounter intakeBottom;
extern LineCounter intakeMiddle;
extern LineCounter intakeTop;
// Distance goalFront;
// Distance goalBack;


/*************************************

Odometry

*************************************/

// VisionOdom visionTest;

//Positioner init
typedef Positioner posTp;

extern GPS_Share share;

extern Positioner positioner;
//Wheel controller
extern Omni_6Controller wc;
extern vision Vision1;
extern vision::signature BLUEGOAL;
extern vision::signature REDGOAL;


/*************************************

Autonomous System Controllers

*************************************/
void graphFlywheelPID(bool remake);
void graphFlywheelTBH(bool remake);

#define TEST_MOT(m) cout << #m << endl; m.spin(fwd); s(1000); m.stop(); s(500);
void testMotorConfiguration();
#define TMC(m) if(!m.installed()){ cout << "Motor " << #m << " is not connected!" << endl; Greg.rumble(".");}
void testMotorConnection();
/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);

struct {
  int count = 0;
  int lastCount = 0;
  bool disabled = false;
  bool clearingDisks = false;
  bool readyState = false;
  bool intaking = false;
  long clearStartTime = 0;
  bool fixingUnstable = false;
  int direction = 0;
  struct {
    bool top = false;
    bool middle = false;
    bool bottom = false;
  } disk;
  vector<LineCounter*> sensors = { &intakeTop, &intakeMiddle, &intakeBottom };
  int diskMask = 0;
  void makeMask(){
    diskMask = 0;
    for(int i = 0; i < sensors.size(); i++){
      diskMask |= (sensors[i]->pressing() << i);
    }
  }
  bool stable(){
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
  bool fixableUnstable(){
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
  void disable(){
    disabled = true;
  }
  void enable(){
    disabled = false;
  }
  void updateValues(bool flywheelReady){
    makeMask();
    
    for(int i = 0; i < sensors.size(); i++){
      ((bool*)&disk)[i] = (diskMask >> i) & 1;
    }
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
  bool hasDisks(){
    return count > 0;
  }
  void setFiring(){
    clearingDisks = true;
    clearStartTime = Brain.Timer.system();
  }
  void intake(){
    intaking = true;
  }
  bool spinMotor(){
    return direction == 1;
  }
  bool reverseMotor(){
    return direction == -1;
  }
} autoIntake;
#endif
