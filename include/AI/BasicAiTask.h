#pragma once
#include "vex.h"
class RobotState
{
public:
    int containedDisks;
    Pose position;
};
class Match
{
public:
    class State
    {
        int timeLeft = 0;
        int otherPoints;
        int currentPoints;
    };

private:
    int timeLeft = 0;
    vex::timer t;
    bool isRunning = false;
    State state;

public:
    Match()
    {
    }
    void start()
    {
        isRunning = true;
        t.reset();
    }
    void stop()
    {
        isRunning = false;
    }
    bool running()
    {
        return isRunning;
    }
    int getTimeLeft()
    {
        return 105 - t.time();
    }
};
class BasicAiTask
{
public:
    virtual void setState(Match::State&) = 0;
    virtual double estimatePointValue() = 0;
    virtual double estimateTime() = 0;
    virtual bool fn() = 0;
    virtual void interrupt() = 0;
    virtual bool callInterrupt() = 0;
    virtual bool done() = 0;
    virtual std::unique_ptr<BasicAiTask> getNew() = 0;
};
class BasicAiTaskBuilder
{
public:
    virtual std::unique_ptr<BasicAiTask> build() = 0;
    virtual double estimatePointValue() = 0;
    virtual double estimateTime() = 0;
};

class AiTaskFactory
{
    LinkedList<std::unique_ptr<BasicAiTaskBuilder>> builders;
    BasicAiTaskBuilder* selectOptimalTask()
    {
        // The gain function is the point value of the task divided by the time it takes to complete the task.
        // Select the task with the highest gain
        // The unit on gain is points / sec
        double maxGain = 0;
        BasicAiTaskBuilder* maxGainTask;
        for (auto& builder : builders)
        {
            double gain = builder->estimatePointValue() / builder->estimateTime();
            if (gain > maxGain)
            {
                maxGain = gain;
                maxGainTask = builder.get();
            }
        }
        return maxGainTask;
    }

public:
    std::unique_ptr<BasicAiTask> createTask()
    {
        // Return the optimal task
        return selectOptimalTask()->build();
    }
    void addBuilder(std::unique_ptr<BasicAiTaskBuilder> builder)
    {
        builders.pushBack(std::move(builder));
    }
};
