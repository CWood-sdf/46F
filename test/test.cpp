#include <iostream>
using namespace std;
#define TEST
#undef NO_MAKE
#include <C:/Users/woodc/OneDrive/GitHub/Libs-Actual/Libs/LinkedList.h>
#include <C:/Users/woodc/OneDrive/GitHub/Libs-Actual/Libs/PVector.h>
#include "PID.cpp"
#include "Bezier.cpp"
#include "Chassis.cpp"
#include "Controllers.cpp"
#include "EPA_Tracker.cpp"
#include "EPA_Wheel_Control.cpp"
#include <stdlib.h>
Chassis chassis = Chassis(15, 1.4, 3.75, gearSetting::ratio18_1);
BasicPidController pidController = BasicPidController(PIDF(6.25, 0.001, 2.4325, 0, 8, 1), PID(6.0, 0.1, 0.1, 0, 0, 10));
PurePursuitController purePursuit = PurePursuitController(-6.25, -0.001, -2.4325, 0, 8, 1);
RamseteController ramsete = RamseteController(0.07, 0.5);
Omni_6Controller wc = Omni_6Controller(&chassis, &ramsete, &purePursuit, &pidController, PID(2.42, 0.2, 1.35, 0, 20, 4), 1.0);
int main(){
    cout << "Hello World" << endl;
    chassis.position.pos = PVector(0, 0);
	chassis.position.angle = 0 * DEG_TO_RAD;
	//Set the path k const to 2  to make the robot go faster
	wc.path.setK(2.0);
	wc.generalFollow(VectorArr({ PVector(0, 48), PVector(48, 48)}), &purePursuit, false);
	//wc.purePursuitFollow(, false);
	// while ((int)chassis.vel != 0 || (int)chassis.angVel != 0) {
	// 	chassis.runSimStep(); s(10);
	// }
    cout << "\x1b[96mDone\033[0m\t\t" << endl;
    return 4;
}