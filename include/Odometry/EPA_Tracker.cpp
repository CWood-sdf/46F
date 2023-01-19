#define NO_MAKE
#include "EPA_Tracker.h"
TrackingWheel::TrackingWheel(bool reverse, double wheelDiameter)
{
    this->reverse = reverse;
    this->wheelDiameter = wheelDiameter;
}
TrackingWheel::TrackingWheel(int32_t port, bool reverse, double wheelDiameter) : TrackingWheel(reverse, wheelDiameter)
{
    rotation* rot = new rotation(port);
    encoder = new Encoder(rot);
    this->rot = rot;
}
TrackingWheel::TrackingWheel(vex::triport::port port, bool reverse, double wheelDiameter) : TrackingWheel(reverse, wheelDiameter)
{
    class encoder* rot = new class encoder(port);
    encoder = new Encoder(rot);
}
TrackingWheel::TrackingWheel(motor& m, bool reverse, double wheelDiameter) : TrackingWheel(reverse, wheelDiameter)
{
    encoder = new Encoder(m);
}
TrackingWheel::TrackingWheel(Encoder& encoder, bool reverse, double wheelDiameter) : TrackingWheel(reverse, wheelDiameter)
{
    this->encoder = &encoder;
}
Positioner::Positioner(encoderArr xArr, encoderArr yArr, PVector fromCenter)
{
    this->fromCenter = fromCenter;
    this->xEncoders = xArr;
    this->yEncoders = yArr;
    for (int i = 0; i < xEncoders.size(); i++)
    {
        if (xEncoders[i].rot != NULL)
        {
            AddDevice("Odom X " + toCcp(i), xEncoders[i].rot);
        }
    }
    for (int i = 0; i < yEncoders.size(); i++)
    {
        if (yEncoders[i].rot != NULL)
        {
            AddDevice("Odom Y " + toCcp(i), yEncoders[i].rot);
        }
    }
}
// Shifts angle to range of [0, 360)
double baseAngle(double ang)
{
    while (ang >= 360.0)
    {
        ang -= 360.0;
    }
    while (ang < 0.0)
    {
        ang += 360.0;
    }
    return ang;
}
// Shifts an angle to a range of (-180, 180]
double posNeg180(double ang)
{
    double ret = baseAngle(ang);
    while (ret > 180.0)
    {
        ret -= 360.0;
    }
    while (ret <= -180.0)
    {
        ret += 360.0;
    }
    return ret;
}
#ifndef WINDOWS
// Function that updates the position
// 80+ lines of trig, vector math, and some sensor stuff
PVector Positioner::update()
{
    // static double lostDist = 0.0;
    // static PVector last = PVector(1, 1);
    // Vector of the wheel angles
    PVector angles = PVector();

    // Get encoder rotation as fast as possible
    // Use raw array for speed
    vector<double> rotX;
    vector<double> rotY;
    // Update bot angle as close to rotation access as possible
    //  updateBotAngle(run);
    if (xEncoders.size() != 0)
    {
        for (int i = 0; i < xEncoders.size(); i++)
        {
            rotX.push_back(xEncoders[i]->position(rotationUnits::deg));
        }
    }
    // cout << yEncoders.size() << endl;
    if (yEncoders.size() != 0)
    {
        for (int i = 0; i < yEncoders.size(); i++)
        {
            rotY.push_back(yEncoders[i]->position(rotationUnits::deg));
        }
    }

    for (int i = 0; i < xEncoders.size(); i++)
    {
        // Get the rotation in radians
        double rot = posNeg180(rotX[i] - lastX[i]) * DEG_TO_RAD * xEncoders[i].mult() * xEncoders[i].wheelRadius();
        // add the change in rotation to angles.x
        angles.x += rot;
        // Reset the last rotation
        lastX[i] = rotX[i];
    }

    // Same thing here
    for (int i = 0; i < yEncoders.size(); i++)
    {
        double rot = posNeg180(rotY[i] - lastY[i]) * DEG_TO_RAD * yEncoders[i].mult() * yEncoders[i].wheelRadius();

        angles.y += rot;
        lastY[i] = rotY[i];
    }

    // Average the angles
    if (yEncoders.size() != 0)
    {
        angles.y /= encYAmnt;
    }
    if (xEncoders.size() != 0)
    {
        angles.x /= encXAmnt;
    }
    PVector deltaAngles;

    if (deltaBotAngle != 0.0)
    {
        double deltaAngle = deltaBotAngle * DEG_TO_RAD;
        double sin2 = 2.0 * sin(deltaAngle / 2.0);
        double x = (angles.x / deltaAngle) * sin2;
        double y = (angles.y / deltaAngle) * sin2;
        deltaAngles = {x, y};
        deltaAngles.rotate(avgBotAngle);
    }
    else
    {
        deltaAngles = angles;
        deltaAngles.rotate(avgBotAngle);
    }

    // Get the change in the position
    PVector deltaPos = deltaAngles;

    speed = deltaPos.dist2D() / (time.time(seconds));
    pos += deltaPos; // Add deltaPos to pos
    return pos;      // Return pos so it can be used
}
PVector Positioner::getPos()
{
    PVector fromCenterCopy = fromCenter;
    fromCenterCopy.rotate(glblBotAngle);
    PVector posCopy = pos;
    posCopy *= -1.0;
    posCopy += fromCenterCopy;
    return posCopy;
}
double Positioner::xPosition(distanceUnits)
{
    return getPos().x;
}
double Positioner::yPosition(distanceUnits)
{
    return getPos().y;
}
double Positioner::heading()
{
    return glblBotAngle;
}
FieldCoord Positioner::fullPos()
{
    return FieldCoord(getPos(), heading());
}
bool Positioner::moving()
{
    // If the velocity is greater than 0.01 in/s or
    // its been less a second since the last call to clearMove
    return abs(speed) > 0.01 ||
           time.time(timeUnits::sec) < 1;
}
double Positioner::velocity()
{
    return speed;
}
void Positioner::clearMove()
{
    time.clear();
}
#endif