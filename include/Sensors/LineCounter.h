
#ifndef LINECOUNTER_H
#define LINECOUNTER_H

#include "Sensors/KillThread.h"
// Oh, this lovely class
// It just always works (unless there's a sun there)

// Class that uses a line tracker to count objects
class LineCounter
{

    static const KillThread updater;
    friend class LineGroup;
    line* sensor;
    bool isActive = false;
    static const int highThresholdPolycarb = 85;
    static const int lowThresholdPolycarb = 85;
    static const int highThresholdAir = 15;
    static const int lowThresholdAir = 13;
    int startThreshold = highThresholdAir;
    int lowThreshold = lowThresholdAir;
    int threshold = startThreshold;
    bool wasActiveLast = false;
    unsigned int countOut = 0;
    unsigned int countIn = 0;
    bool fHit = false;

public:
    static vector<LineCounter*> instances;
    LineCounter(const LineCounter&) = delete;
    LineCounter(line& se, bool throughPolycarb = false) : sensor(&se)
    {
        instances.push_back(this);
        if (throughPolycarb)
        {
            startThreshold = highThresholdPolycarb;
            lowThreshold = lowThresholdPolycarb;
        }
    }
    LineCounter(triport::port& p, bool throughPolycarb = false) : sensor(new line(p))
    {
        instances.push_back(this);
        if (throughPolycarb)
        {
            startThreshold = highThresholdPolycarb;
            lowThreshold = lowThresholdPolycarb;
        }
    }
    // Return true if its the first time called after a hit
    bool firstHit();
    // Return true if obj near
    bool active();
    // Update the values
    void update();
    // Reset the counts
    void reset();
    // Get the number of objs that entered field of view, then left
    int getCountOut();
    // Get the number of objs that entered field of view
    int getCountIn();
    // Same as active()
    bool pressing();
    // Returns raw line tracker value
    int rawData();
};
void microWait(uint time);

#endif // LINECOUNTER_H
