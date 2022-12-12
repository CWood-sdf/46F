// #include "EPA_Tracker.h"
#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H
#include "Sensors/PotDial.h"
#include "Sensors/LineCounter.h"
// #include "Sensors/Distance.h"
#include "Sensors/VisionOdom.h"
#include "Odometry/Controllers.h"
#include "Flywheel/Controller.h"
#include "Sensors/Wrappers/Encoder.h"
#include "IntakeAutomation.h"
/*************************************

Motors

*************************************/
// //Front Left Wheel (FL)
// extern motor FL;
// //Front Right Wheel (FR)
// extern motor FR;
// //Back Left Wheel (BL)
// extern motor BL;
// //Back Right Wheel (BR)
// extern motor BR;
// //Middle Left Wheel (ML)
// extern motor ML;
// //Middle Right Wheel (MR)
// extern motor MR;

// extern Pto leftPto;
// extern Pto rightPto;

// extern motor_group Left;
// extern motor_group Right;

// extern motor flyWheelMot;
// extern motor flywheel2;

extern NewMotor intake;

extern NewMotor flywheelNm;
// extern FlywheelPID flyPID;
extern FlywheelTBHEncoder flyTBH;
// extern bool flywheelPID;
// New Motors, a few reasons for this:
//    1 - less upfront code for stuff
//    2 - Simplified spin cmd
// extern NewMotor wheels;
extern NewMotor leftWheels;
extern NewMotor rightWheels;

extern Chassis chassis;
extern PurePursuitController purePursuit;
extern RamseteController ramsete;
extern BasicPidController pidController;
extern DebugController debugController;

/*************************************

Pneumatics

*************************************/
extern pneumatics endgame;
extern pneumatics goalRaise;
// extern pneumatics pto;

/*************************************

Sensors

*************************************/

// Three wire expander
extern triport Expander;

// Inertial Sensor
extern inertial angler;

// gps
extern gps GPS;

extern optical rachetColor;

extern LineCounter intakeBottom;
extern vex::distance intakeMiddle;
extern LineCounter intakeTop;
extern AutoIntake intakeController;
// Distance goalFront;
// Distance goalBack;

/*************************************

Odometry

*************************************/

// VisionOdom visionTest;

// Positioner init
typedef Positioner posTp;

extern GPS_Share share;

extern Positioner positioner;
// Wheel controller
extern WheelController wc;
extern vision Vision1;
extern vision::signature BLUEGOAL;
extern vision::signature REDGOAL;

/*************************************

Autonomous System Controllers

*************************************/
void graphFlywheelPID(bool remake);
void graphFlywheelTBH(bool remake);

#define TEST_MOT(m)     \
    cout << #m << endl; \
    m.spin(fwd);        \
    s(1000);            \
    m.stop();           \
    s(500);
void testMotorConfiguration();
#define TMC(m)                                                  \
    if (!m.installed())                                         \
    {                                                           \
        cout << "Motor " << #m << " is not connected!" << endl; \
        Greg.rumble(".");                                       \
    }
void testMotorConnection();
/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);
#endif