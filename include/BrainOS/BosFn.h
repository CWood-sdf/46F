#include "vex.h"
class BosFn
{
    bool (*popFn)(bool);
    void (*mainFn)(bool);
    bool isPop = false;

public:
    BosFn() = delete;
    BosFn(bool (*p)(bool))
    {
        popFn = p;
        isPop = true;
    }
    BosFn(void (*m)(bool))
    {
        mainFn = m;
        isPop = false;
    }

    bool call(bool remake = false)
    {
        if (isPop)
        {
            return popFn(remake);
        }
        else
        {
            mainFn(remake);
        }
        return false;
    }
    bool operator==(const BosFn& other) const
    {
        return isPop ? popFn == other.popFn : mainFn == other.mainFn;
    }
};