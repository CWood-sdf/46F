#ifndef INTAKE_AUTOMATION_H
#define INTAKE_AUTOMATION_H
#include "Sensors/LineCounter.h"

struct AutoIntake
{
  int count = 0;
  int lastCount = 0;
  bool disabled = false;
  bool clearingDisks = false;
  bool clearingLastDisk = false;
  bool readyState = false;
  bool intaking = false;
  long clearStartTime = 0;
  bool fixingUnstable = false;
  int direction = 0;
  //   struct {
  //     bool top = false;
  //     bool middle = false;
  //     bool bottom = false;
  //   } disk;
  vector<std::function<bool()>> sensors;
  int diskMask = 0;
  AutoIntake(vector<std::function<bool()>> sensors);
  /**
   * @brief For internal use only, stores all the sensor values in a mask for easy comparison
   *
   */
  void makeMask();
  /**
   * @brief Checks if the disks are ready for intaking
   *
   * @return true yes they are
   * @return false no they are not
   */
  bool stable();
  /**
   * @brief Checks if the disks can be moved to a "stable" position
   *
   * @return true yes they can
   * @return false no they cant
   */
  bool fixableUnstable();
  /**
   * @brief Disables the auto intake
   *
   */
  void disable();
  /**
   * @brief Enables the auto intake
   *
   */
  void enable();
  /**
   * @brief Updates the values of the auto intake
   *
   * @param flywheelReady should be true if the flywheel is ready to fire
   */
  void updateValues(bool flywheelReady);
  /**
   * @brief Checks if the intake has disks
   *
   * @return true
   * @return false
   */
  bool hasDisks();
  /**
   * @brief Set the intake to fire mode
   *
   */
  void setFiring();
  /**
   * @brief Set the intake to intake mode
   *
   */
  void intake();
  /**
   * @brief Set intaking for multiple disks
   *
   */
  void intakeMultiple(int count);
  /**
   * @brief Return true if the intake should move up
   *
   * @return true
   * @return false
   */
  bool spinMotor();
  /**
   * @brief Return true if the intake should move down
   *
   * @return true
   * @return false
   */
  bool reverseMotor();
  /**
   * @brief Waits for intake to be done firing
   *
   */
  void waitForFiring();
  /**
   * @brief Waits for intake to be done intaking
   *
   */
  void waitForIntake();
  /**
   * @brief Sets the intake to be ready for auton
   *
   */
  void autonInit();
};

#endif