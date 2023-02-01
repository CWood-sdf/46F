#ifndef CHASSIS_H
#define CHASSIS_H
#include "Bezier.h"
#include "EPA_Tracker.h"
#include "PID.h"

struct Chassis;
class Controller;
class Path
{
public:
    struct El
    {
        PVector bezierPt;
        double targetSpeed;
        double targetAngle;
        double curvature;
        operator PVector()
        {
            return bezierPt;
        }
    };

private:
    typedef Path& chain_method;
    vector<El> path;
    VectorArr arr;
    double kConst;

public:
    chain_method setK(double s);
    void make(VectorArr& arr, Chassis* chassis);
    void remake(Chassis* chassis);
    VectorArr getBezier();
    vector<Ref<PVector>> getBezierRef();
    int size();
    El& last();
    El& operator[](int index);
};
struct Chassis
{
    typedef Chassis& chain_method;
    double speedLimit = 100;
    double maxAcc = 100; // in/s^2
    double maxDAcc = 80; // in/s^2
    MotorGroup& leftWheels;
    MotorGroup& rightWheels;
    Positioner& pos;
    double trackWidth = 0.0;
    double gearRatio = 1.0;
    double wheelRad = 0.0;
    gearSetting cartridge = gearSetting::ratio18_1;
    double botAngle();
    PVector botPos();
    double revToInches(double rotation)
    {
        return rotation * 2.0 * M_PI * wheelRad / gearRatio;
    }
    double inchesToRev(double inches)
    {
        return inches * gearRatio / 2.0 / M_PI / wheelRad;
    }
    // converts pct to in/s
    double pctToReal(double speed)
    {
        // rpm
        double motorVel;
        switch (cartridge)
        {
        case vex::gearSetting::ratio18_1:
            motorVel = 200;
            break;
        case vex::gearSetting::ratio36_1:
            motorVel = 100;
            break;
        case vex::gearSetting::ratio6_1:
            motorVel = 600;
            break;
        }
        return motorVel * speed / 100.0 / gearRatio * 2.0 * M_PI / 60.0 /*rad/s*/
               * wheelRad;
    }
    double realToPct(double speed)
    {
        // rpm
        double motorVel;
        switch (cartridge)
        {
        case vex::gearSetting::ratio18_1:
            motorVel = 200;
            break;
        case vex::gearSetting::ratio36_1:
            motorVel = 100;
            break;
        case vex::gearSetting::ratio6_1:
            motorVel = 600;
            break;
        }
        return speed / motorVel * 100.0 * gearRatio / 2.0 / M_PI * 60.0 / wheelRad;
    }
    void driveFromCurvature(double speed, double curvature);
    void driveFromAngular(double speed, double angularVel);
    void driveFromLR(double left, double right);
    void driveFromDiff(double speed, double diff, directionType d);
    void hardBrake();
    void coastBrake();
    void turnLeft(double speed);
    void turnRight(double speed);
    void moveLeftSide(double speed, directionType d);
    void moveRightSide(double speed, directionType d);
    void move(double speed, directionType d);
    chain_method setMaxAcc(double v);
    chain_method setMaxDAcc(double v);
    chain_method setSpeedLimit(double v);
    Chassis(MotorGroup& left, MotorGroup& right, Positioner& p, double trackWidth, double gearRatio, double wheelRad, gearSetting cartridge);
};
#endif
