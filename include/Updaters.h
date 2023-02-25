#include "robot-config.h"
#ifdef MAKE

void microWait(uint);
void executeThreads();
void updatePos();
#else
void microWait(uint time) {
    auto startTime = Brain.Timer.systemHighResolution();
    int i = 0;
    while (Brain.Timer.systemHighResolution() - startTime < time || i < -10) {
        i++;
    }
}
void updatePos() {
    while (1) {
        positioner.update();
    }
}
#endif
