#pragma once
#include "vex.h"
class Match {
  int timeLeft = 0;
  vex::timer t;
  bool isRunning = false;
public:
  Match() {
    
  }
  void start() {
    isRunning = true;
    t.reset();
  }
  void stop() {
    isRunning = false;
  }
  bool running() {
    return isRunning;
  }
  int getTimeLeft() {
    return 105 - t.time();
  }
};
class BasicAiTask {
public:
  static virtual double estimatePointValue() = 0;
  static virtual double estimateTime() = 0;
  virtual bool fn() = 0;
  virtual void interrupt() = 0;
  virtual bool callInterrupt() = 0;
  virtual bool done() = 0;
  virtual std::unique_ptr<BasicAiTask> getNew() = 0;
};
class BasicAiTaskBuilder {
public:
  virtual std::unique_ptr<BasicAiTask> build() = 0;
  virtual double estimatePointValue() = 0;
  virtual double estimateTime() = 0;
};

class AiTaskFactory {
  LinkedList<std::unique_ptr<BasicAiTaskBuilder>> builders;
  std::unique_ptr<BasicAiTaskBuilder> selectOptimalTask() {
    //The gain function is the point value of the task multiplied by the time it takes to complete the task.
    //Select the task with the highest gain
    double maxGain = 0;
    std::unique_ptr<BasicAiTaskBuilder> maxGainTask;
    for (auto& builder : builders) {
      double gain = builder->estimatePointValue() * builder->estimateTime();
      if (gain > maxGain) {
        maxGain = gain;
        maxGainTask = builder;
      }
    }
    return maxGainTask;
  }
public:
  std::unique_ptr<BasicAiTask> createTask() {
    //Return the optimal task
    return selectOptimalTask()->build();
  }
  void addBuilder(std::unique_ptr<BasicAiTaskBuilder> builder) {
    builders.push_back(std::move(builder));
  }
};