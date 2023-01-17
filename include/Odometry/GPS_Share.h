#ifndef GPS_SHARE_H
#define GPS_SHARE_H
#include "EPA_Tracker.h"
#include "Filters.h"
#include <memory>
#ifndef WINDOWS
class BasicConfirmationOdom
{
public:
    virtual ~BasicConfirmationOdom() {}
    virtual double xPosition(distanceUnits) = 0;
    virtual double yPosition(distanceUnits) = 0;
    virtual int32_t quality() = 0;
    virtual bool installed() = 0;
    virtual double variance() = 0;
    virtual void updateEstimate() = 0;
};
// Sleeps itself because it is top level odometry math
class GPS_Share
{
    Positioner& odom;
    gps& GPS;
    BasicEMA<FieldCoord> gpsEma;
    LinkedList<FieldCoord> gpsReadings;
    LinkedList<FieldCoord> odomReadings;
    vector<std::unique_ptr<BasicConfirmationOdom>> confirmOdoms;
    FieldCoord pos = FieldCoord(PVector(0, 0), 0);
    FieldCoord lastOdom = FieldCoord(PVector(0, 0), 0);
    long long lastGpsUpdate = 0;
    double speed = 0.0;
    EMA speedFilter = EMA(1, 0);
    bool lastBad = false;
    bool useGps = false;
    bool isFirstBad = false;
    static const int sleepTime = 10;
    static const int badTime = 200;
    // returns true if it's first call since reading gone bad
    // Resets when a good reading is obtained
    bool firstBad();
    bool readingBad();

public:
    GPS_Share(Positioner& o, gps& g);

    FieldCoord& fullPos();
    PVector& position();
    double heading();
    double velocity();
    bool gpsBad();
    void update();
    void setPos(PVector v, double a);
    void setUseGps(bool s)
    {
        useGps = s;
    }
    long long timeSinceLastGpsMs()
    {
        return Brain.Timer.system() - lastGpsUpdate;
    }
};
#endif
#endif