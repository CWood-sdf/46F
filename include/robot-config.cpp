#define NO_MAKE
#include "robot-config.h"
//Front Left Wheel (FL)
motor FL = motor(PORT1,gearSetting::ratio18_1, true);
//Front Right Wheel (FR)
motor FR = motor(PORT5, gearSetting::ratio18_1,false);
//Back Left Wheel (BL)
motor BL = motor(PORT3, gearSetting::ratio18_1,true);
//Back Right Wheel (BR)
motor BR = motor(PORT9, gearSetting::ratio18_1,false);
//Middle Left Wheel (ML)
motor ML = motor(PORT2, gearSetting::ratio18_1,true);
//Middle Right Wheel (MR)
motor MR = motor(PORT8, gearSetting::ratio18_1,false);

motor_group Left = motor_group(BL, ML, FL);
motor_group Right = motor_group(BR, MR, FR);

motor intakeMot = motor(PORT7, gearSetting::ratio18_1, true);

motor flyWheelMot = motor(PORT10, gearSetting::ratio6_1, true);
motor flywheel2 = motor(PORT6, ratio6_1, false);
NewMotor<> flywheelNm = NewMotor<>(flyWheelMot, flywheel2);
encoder flySensor = encoder(Brain.ThreeWirePort.A);
FlywheelPID flyPID = FlywheelPID(flyWheelMot, flySensor);
FlywheelTBH flyTBH = FlywheelTBH(flywheelNm, flySensor);
bool flywheelPID = false;
//New Motors, a few reasons for this: 
//    1 - less upfront code for stuff
//    2 - Simplified spin cmd
NewMotor<> wheels = NewMotor<>(FL, ML, BL, FR, MR, BR);
NewMotor<> leftWhls = NewMotor<>(BL, FL, ML);
NewMotor<> rghtWhls = NewMotor<>(BR, FR, MR);

NewMotor<> intake = NewMotor<>(intakeMot);



/*************************************

Pneumatics

*************************************/


/*************************************

Sensors

*************************************/


//Three wire expander
triport Expander = triport(PORT9);

//Inertial Sensor
inertial angler = inertial(PORT11);

//gps
gps GPS = gps(PORT10, -6.0, 0.0, inches, -90);

optical rachetColor = optical(PORT8);

LineCounter rachetDetector (Brain.ThreeWirePort.A);
// Distance goalFront = Distance(PORT11);
// Distance goalBack = Distance(PORT12);

/*************************************

Odometry

*************************************/

// VisionOdom visionTest = VisionOdom(goalFrontVision, {0, 0}, 12.5, 32, 0);

//Positioner init
posTp::xPortArr arrX = { };
posTp::yPortArr arrY = { Brain.ThreeWirePort.E };
//Make a positioner that measures x and y with smallest omni wheel rad
posTp positioner = posTp(arrX, arrY, 
                        { 1.0 }, { 1.0 }, { 1.0 }, { 1.0 },
                         0.0 ,  0.0 ,
                        1.375);

GPS_Share share = GPS_Share(positioner, GPS);

//Wheel controller

Chassis chassis = Chassis({BL, ML, FL}, {BR, MR, FR}, share, 15, 1, 3.75, gearSetting::ratio18_1);
PurePursuitController purePursuit = PurePursuitController(6.25, 0.001, 2.4325, 0, 8, 1);
RamseteController ramsete = RamseteController(1.0, 0.5);
BasicPidController pidController = BasicPidController(PIDF(6.25, 0.001, 2.4325, 0, 8, 1), PID(6.0, 0.1, 0.1, 0, 0, 10));

Omni_6Controller wc = Omni_6Controller(&chassis, &ramsete, &purePursuit, PID(2.42, 0.2, 1.35, 0, 20, 4), 1.0);

/*************************************

Autonomous System Controllers

*************************************/
void graphFlywheelPID(bool remake){
  if(remake){
    flywheelPID = true;
  }
  flyPID.graph(remake);
}
void graphFlywheelTBH(bool remake){
  if(remake){
    flywheelPID = false;
  }
  flyTBH.graph(remake);
}

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

bool toBool(double v){
  return (bool)(int)(v + 0.5);
}