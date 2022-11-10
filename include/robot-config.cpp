#define NO_MAKE
#include "robot-config.h"
std::vector<tuple<string, vex::device*>> connectedDevices = {};

AddDevice::AddDevice(string name, vex::device* device){
  connectedDevices.push_back(make_tuple(name, device));
}
#define TestDevice(device) AddDevice device##AddDevice(#device, &device);
#define TestDevices(device, ...) TestDevice(device); TestDevices(__VA_ARGS__)
//Make a brain
brain Brain;

//Make a controller and name it Greg
controller Greg = controller();
controller Beethoven = controller(partner);

//Front Left Wheel (FL)
motor FL = motor(PORT19, gearSetting::ratio18_1, !true);
TestDevice(FL);
//Front Right Wheel (FR)
motor FR = motor(PORT9, gearSetting::ratio18_1, !false);
TestDevice(FR);
//Back Left Wheel (BL)
motor BL = motor(PORT18, gearSetting::ratio18_1, !true);
TestDevice(BL);
//Back Right Wheel (BR)
motor BR = motor(PORT12, gearSetting::ratio18_1, !false);
TestDevice(BR);
//Middle Left Wheel (ML)
// motor ML = motor(PORT17, gearSetting::ratio18_1, true);
// TestDevice(ML);
// //Middle Right Wheel (MR)
// motor MR = motor(PORT8, gearSetting::ratio18_1, false);
// TestDevice(MR);

motor_group Left = motor_group(BL, FL);
motor_group Right = motor_group(BR, FR);

motor intakeMot = motor(PORT17, gearSetting::ratio18_1, false);
TestDevice(intakeMot);
motor intakeMot2 = motor(PORT5, gearSetting::ratio18_1, true);
TestDevice(intakeMot2);

motor flyWheelMot = motor(PORT3, gearSetting::ratio6_1, false);
TestDevice(flyWheelMot);
motor flywheel2 = motor(PORT4, ratio6_1, true);
TestDevice(flywheel2);
NewMotor flywheelNm = NewMotor(flyWheelMot, flywheel2);
// encoder flySensor = encoder(Brain.ThreeWirePort.A);
// rotation flywheelRotation = rotation(PORT5);
Encoder e = Encoder(flyWheelMot);
// FlywheelPID flyPID = FlywheelPID(flyWheelMot, flySensor);
FlywheelTBHEncoder flyTBH = FlywheelTBHEncoder(flywheelNm, e);
// bool flywheelPID = false;
//New Motors, a few reasons for this: 
//    1 - less upfront code for stuff
//    2 - Simplified spin cmd
// NewMotor wheels = NewMotor(FL, ML, BL, FR, MR, BR);
NewMotor leftWhls = NewMotor(BL, FL);
NewMotor rghtWhls = NewMotor(BR, FR);

NewMotor intake = NewMotor(intakeMot, intakeMot2);



// /*vex-vision-config:begin*/
// vex::vision::signature BLUEGOAL = vex::vision::signature (1, -1873, 915, -479, 3997, 7001, 5499, 2.5, 0);
// vex::vision::signature REDGOAL = vex::vision::signature (2, 7225, 9019, 8122, -879, 497, -191, 3, 0);
// vex::vision::signature SIG_3 = vex::vision::signature (3, 0, 0, 0, 0, 0, 0, 2.5, 0);
// vex::vision::signature SIG_4 = vex::vision::signature (4, 0, 0, 0, 0, 0, 0, 2.5, 0);
// vex::vision::signature SIG_5 = vex::vision::signature (5, 0, 0, 0, 0, 0, 0, 2.5, 0);
// vex::vision::signature SIG_6 = vex::vision::signature (6, 0, 0, 0, 0, 0, 0, 2.5, 0);
// vex::vision::signature SIG_7 = vex::vision::signature (7, 0, 0, 0, 0, 0, 0, 2.5, 0);
// vex::vision Vision1 = vex::vision (vex::PORT1, 66, BLUEGOAL, REDGOAL, SIG_3, SIG_4, SIG_5, SIG_6, SIG_7);
// /*vex-vision-config:end*/

/*************************************

Pneumatics

*************************************/

pneumatics endgame = pneumatics(Brain.ThreeWirePort.C);
/*************************************

Sensors

*************************************/


//Three wire expander
// triport Expander = triport(PORT9);
// TestDevice(Expander);
//Inertial Sensor
inertial angler = inertial(PORT16);
TestDevice(angler);

//gps
gps GPS = gps(PORT7, 6, 4.0, inches, 0);
TestDevice(GPS);

optical rachetColor = optical(PORT11);
TestDevice(rachetColor);

LineCounter rachetDetector (Brain.ThreeWirePort.A);
// Distance goalFront = Distance(PORT11);
// Distance goalBack = Distance(PORT12);

/*************************************

Odometry

*************************************/

// VisionOdom visionTest = VisionOdom(goalFrontVision, {0, 0}, 12.5, 32, 0);

//Positioner init
posTp::xPortArr arrX = { };

posTp::yPortArr arrY = { Port(PORT15) };
//Make a positioner that measures x and y with smallest omni wheel rad
posTp positioner = posTp(arrX, arrY, 
                        { -1.0 }, { -1.0 }, { -1.0 }, { -1.0 },
                         0.0 ,  0.0 ,
                        1.375);

GPS_Share share = GPS_Share(positioner, GPS);

//Wheel controller

Chassis chassis = Chassis(leftWhls, rghtWhls, share, 11.25, 36.0/60.0, 3.75, gearSetting::ratio6_1);
PurePursuitController purePursuit = PurePursuitController(PID(6.25, 0.001, 2.4325, 0, 8, 1));
RamseteController ramsete = RamseteController(0.0108, 0.2);
BasicPidController pidController = BasicPidController(PIDF(6.25, 0.1, 2.4325, 20, 6, 1), PID(1.0, 0, 0.3, 0, 0, 0));
Omni_6Controller wc = Omni_6Controller(&chassis, &ramsete, &purePursuit, &pidController, PID(2.42, 0.2, 1.35, 0, 20, 4), 1.0);

/*************************************

Autonomous System Controllers

*************************************/
// void graphFlywheelPID(bool remake){
//   if(remake){
//     flywheelPID = true;
//   }
//   flyPID.graph(remake);
// }
void graphFlywheelTBH(bool remake){
  if(remake){
    // flywheelPID = false;
  }
  flyTBH.graph(remake);
}

#define TEST_MOT(m) cout << #m << endl; m.spin(fwd); s(1000); m.stop(); s(500);
void testMotorConfiguration(){
  TEST_MOT(FL)
  // TEST_MOT(ML)
  TEST_MOT(BL)
  s(1000);
  TEST_MOT(FR)
  // TEST_MOT(MR)
  TEST_MOT(BR)
}
#define TMC(m) if(!m.installed()){ cout << "Motor " << #m << " is not connected!" << endl; Greg.rumble(".");}
void testMotorConnection(){
  // TMC(FL)
  // TMC(ML)
  // TMC(BL)
  // TMC(FR)
  // TMC(MR)
  // TMC(BR)
  for(auto i : connectedDevices){
    if(!get<1>(i)->installed()){\
      int32_t port = get<1>(i)->index() + 1;
      cout << get<0>(i) << " is not connected on port " << port << "!" << endl;
      Greg.rumble(".");
    }
  }
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