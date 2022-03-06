//#include "EPA_Tracker.h"
#include "Sensors/PotDial.h"
#include "Sensors/LineCounter.h"
#include "Sensors/Distance.h"
#include "Sensors/VisionOdom.h"
#include "Odometry/EPA_Wheel_Control.h"
/*************************************

Motors

*************************************/
//Front Left Wheel (FL)
motor FL = motor(PORT12,gearSetting::ratio18_1,!false);
//Front Right Wheel (FR)
motor FR = motor(PORT19, gearSetting::ratio18_1,!true);
//Back Left Wheel (BL)
motor BL = motor(PORT16, gearSetting::ratio18_1,!false);
//Back Right Wheel (BR)
motor BR = motor(PORT9, gearSetting::ratio18_1,!true);
//Middle Left Wheel (ML)
motor ML = motor(PORT14, gearSetting::ratio18_1,!false);
//Middle Right Wheel (MR)
motor MR = motor(PORT13, gearSetting::ratio18_1,!true);

motor_group Left = motor_group(BL, ML, FL);
motor_group Right = motor_group(BR, MR, FR);


//New Motors, a few reasons for this: 
//    1 - less upfront code for stuff
//    2 - Simplified spin cmd
NewMotor<> wheels = NewMotor<>(FL, ML, BL, FR, MR, BR);
NewMotor<> leftWhls = NewMotor<>(BL, FL, ML);
NewMotor<> rghtWhls = NewMotor<>(BR, FR, MR);
/*************************************

Pneumatics

*************************************/


/*************************************

Sensors

*************************************/
//Front Goal Switch
LineCounter frontCounter(Brain.ThreeWirePort.C);
// LineCounter backCounter(Brain.ThreeWirePort.F);

//Three wire expander
triport Expander = triport(PORT9);

//Inertial Sensor
inertial angler = inertial(PORT11);

//gps
gps GPS = gps(PORT18, -6.0, 0.0, inches, -90);
// Distance goalFront = Distance(PORT11);
// Distance goalBack = Distance(PORT12);


//PotDials
// PotDial skillsOrSide = PotDial(Brain.ThreeWirePort.C, 3);
// PotDial centerGoals = PotDial(Brain.ThreeWirePort.D, 4);
// PotDial autonGoals = PotDial(Brain.ThreeWirePort.E, 2);
// PotDial::Controller<function<void()>> autonMap = 
//     PotDial::Controller<function<void()>>(skillsOrSide, centerGoals, autonGoals);


/*************************************

Odometry

*************************************/

// VisionOdom visionTest = VisionOdom(goalFrontVision, {0, 0}, 12.5, 32, 0);

//Positioner init
typedef Positioner posTp;
posTp::xPortArr arrX = { };
posTp::yPortArr arrY = { Brain.ThreeWirePort.E };
//Make a positioner that measures x and y with smallest omni wheel rad
posTp positioner = posTp(arrX, arrY, 
                        { 1.0 }, { 1.0 }, { 1.0 }, { 1.0 },
                         0.0 ,  0.0 ,
                        1.375);

GPS_Share share = GPS_Share(positioner, GPS);

//Wheel controller
Omni_6Controller wc = Omni_6Controller(BL, BR, FL, FR, ML, MR, share);


/*************************************

Autonomous System Controllers

*************************************/


#define TEST_MOT(m) cout << #m << endl; m.spin(fwd); s(1000); m.stop(); s(500);
void testMotorConfiguration(){
  TEST_MOT(FL)
  TEST_MOT(ML)
  TEST_MOT(BL)
  s(1000);
  TEST_MOT(FR)
  TEST_MOT(MR)
  TEST_MOT(BR)
}
#define TMC(m) if(!m.installed()){ cout << "Motor " << #m << " is not connected!" << endl; Greg.rumble(".");}
void testMotorConnection(){
  TMC(FL)
  TMC(ML)
  TMC(BL)
  TMC(FR)
  TMC(MR)
  TMC(BR)
}
/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void){
  // wheels.set(hold);
}
