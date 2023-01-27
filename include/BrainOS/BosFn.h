#include "vex.h"
class BosFn
{
    bool (*popFn)(bool);
    void (*mainFn)(bool);
    bool isLvgl = false;
    bool isPop = false;

public:
    BosFn()
    {
    }
    BosFn(bool (*p)(bool), bool isLv = false)
    {
        isLvgl = isLv;
        popFn = p;
        isPop = true;
    }
    BosFn(void (*m)(bool), bool isLv = false)
    {
        isLvgl = isLv;
        mainFn = m;
        isPop = false;
    }
    bool lvgl()
    {
        return isLvgl;
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