#ifndef POTDIAL_H
#define POTDIAL_H

#include "Odometry/EPA_Tracker.h"
typedef triport::port port;
class PotDial;
class SelectorArr
{
    typedef function<void()> FnTp;
    static inline FnTp emptyFn = []() {};
    static inline vector<PotDial*> boundDials = {};
    static inline map<vector<int>, FnTp> toVal = {};
    vector<int> count;
    bool isValid = false;
    friend class PotDial;
    static void addDial(PotDial* dial);

public:
    SelectorArr(vector<int> count, FnTp fn);
    SelectorArr(vector<int> count);
    SelectorArr();
    void attachFn(FnTp fn);
    static FnTp getVal();
};
class PotDial
{
    const int ticks;
    pot* sensor;
    double baseVal = 0.0;
    int range = 250;

    PotDial(int tickAmnt) : ticks(tickAmnt)
    {
        SelectorArr::addDial(this);
    }
    PotDial(int tickAmnt, int rng) : PotDial(tickAmnt)
    {
        range = rng;
    }
    PotDial(int tickAmnt, int rng, double baseVal) : PotDial(tickAmnt, rng)
    {
        this->baseVal = baseVal;
    }

public:
public:
    template <typename... Args>
    PotDial(pot& sensor, Args... otherArgs) : PotDial(otherArgs...)
    {
        this->sensor = &sensor;
    }
    template <typename... Args>
    PotDial(port& p, Args... args) : PotDial(args...)
    {
        sensor = new pot(p);
    }
    int getAmnt();
};

#endif // POTDIAL_H
