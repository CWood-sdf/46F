#define NO_MAKE
#include "IntakeAutomation.h"

AutoIntake::AutoIntake(vector<std::function<bool()>> sensors)
{
  this->sensors = sensors;
}
void AutoIntake::makeMask()
{
  diskMask = 0;
  for (int i = 0; i < sensors.size(); i++)
  {
    diskMask |= (sensors[i]() << 2 * i);
  }
}
bool AutoIntake::stable()
{
  // Is stable if it does not end with 0's
  //  (i.e. return true if 0b011 or 0b001 or 0b111 or 0b000 but not 0b010 or 0b100)
  int lastBit = 1;
  for (int i = 0; i < sensors.size(); i++)
  {
    int bit = (diskMask >> i) & 1;
    if (bit == 1 && lastBit == 0)
    {
      return false;
    }
    lastBit = bit;
  }
  return true;
}
bool AutoIntake::fixableUnstable()
{
  // Is fixable if it ends with 0's
  //  (i.e. return true if 0b010 or 0b100 but not 0b011 or 0b001 or 0b111 or 0b000 or 0b101)
  int lastBit = 0;
  for (int i = 0; i < sensors.size(); i++)
  {
    int bit = (diskMask >> i) & 1;
    if (bit == 0 && lastBit == 1)
    {
      return false;
    }
    lastBit = bit;
  }
  return true;
}

void AutoIntake::disable()
{
  disabled = true;
}

void AutoIntake::enable()
{
  disabled = false;
}

void AutoIntake::updateValues(bool flywheelReady)
{
  static int i = 0;
  makeMask();
  // Mask correction
  if (diskMask != lastMask)
  {
    if ((diskMask & 0b10000) == 0 && (lastMask & 0b10000) == 1)
    {
      if (direction == 1)
      {
        // Disk has exited
      }
      if (direction == -1)
      {
        // Move disk to interstage, only if there isn't a disk there
        if ((lastMask & 0b1000) == 0)
        {
          // Move disk to interstage
          diskMask |= 0b1000;
        }
        else
        {
          // Leave disk up top
          diskMask |= 0b10000;
        }
      }
    }
    if ((diskMask & 0b100) == 0 && (lastMask & 0b100) == 1)
    {
      if (direction == 1)
      {
        // Move disk to middle interstage, only if there isn't a disk there
        if ((lastMask & 0b1000) == 0)
        {
          // Move disk to interstage
          diskMask |= 0b1000;
        }
        else
        {
          // Leave disk there
          diskMask |= 0b100;
        }
      }
      if (direction == -1)
      {
        // Move disk to interstage, only if there isn't a disk there
        if ((lastMask & 0b10) == 0)
        {
          // Move disk to interstage
          diskMask |= 0b10;
        }
        else
        {
          // Leave disk there
          diskMask |= 0b100;
        }
      }
    }
    if ((diskMask & 0b1) == 0 && (lastMask & 0b1) == 1)
    {
      if (direction == 1)
      {
        // Move disk to bottom interstage, only if there isn't a disk there
        if ((lastMask & 0b10) == 0)
        {
          // Move disk to interstage
          diskMask |= 0b10;
        }
        else
        {
          // Leave disk there
          diskMask |= 0b1;
        }
      }
      if (direction == -1)
      {
        // Disk is out
      }
    }
  }
  lastMask = diskMask;
  int count = 0;
  for (int i = 0; i < sensors.size() * 2 - 1; i++)
  {
    count += (diskMask >> i) & 1;
  }
  // cout << "Intaking: " << intaking << endl;
  if (intaking && sensors.back()())
  {
    intaking = false;
  }
  if (intaking && count == lastCount + 1)
  {
    intaking = false;
  }
  if (clearingDisks && count == 0 && clearingLastDisk)
  {
    clearingDisks = false;
    clearingLastDisk = false;
  }
  int lastDisk = 1 << (2 * sensors.size() - 2);
  if (diskMask == lastDisk && clearingDisks)
  {
    clearingLastDisk = true;
  }
  else
  {
    clearingLastDisk = false;
  }
  if ((diskMask & lastDisk) == lastDisk && clearingDisks)
  {
    tempClearReady = true;
  }
  else
  {
    tempClearReady = false;
  }
  if (clearingDisks && tempClearReady && (diskMask & lastDisk) == 0)
  {
    tempClearStop = 200;
    tempClearReady = false;
  }
  if (tempClearStop > 0)
  {
    tempClearStop -= 10;
  }
  // If seconds of clearing disks, then stop
  if (clearingDisks && (Brain.Timer.system() - clearStartTime) > 10000)
  {
    // clearingDisks = false;
  }
  direction = 0;
  if (intaking || (clearingDisks && (flywheelReady || !sensors.back()()) && tempClearStop <= 0))
  {
    direction = 1;
  }
  if (direction != 1 && ((!stable() && fixableUnstable()) || fixingUnstable) && !clearingDisks)
  {
    direction = -1;
    fixingUnstable = true;
  }
  if (i++ == 30)
  {
    // cout << "Mask: " << diskMask << endl;
    // cout << "Stable: " << stable() << endl;
    // cout << "Fixable: " << fixableUnstable() << endl;
    // cout << "Count: " << count << endl;
    // cout << "Direction: " << direction << endl;
    i = 0;
  }
  if (fixingUnstable && stable() && count == 0)
  {
    fixingUnstable = false;
  }
  if (direction != 0)
  {
    lastCount = count;
  }
}

bool AutoIntake::hasDisks()
{
  return count > 0;
}

void AutoIntake::setFiring()
{
  clearingDisks = true;
  clearStartTime = Brain.Timer.system();
}

void AutoIntake::intake()
{
  intaking = true;
}

void AutoIntake::intakeMultiple(int count)
{
  intake();
  lastCount += count - 1;
  if (lastCount >= 3)
  {
    lastCount = 2;
  }
}

bool AutoIntake::spinMotor()
{
  return direction == 1;
}

bool AutoIntake::reverseMotor()
{
  return direction == -1;
}

void AutoIntake::waitForFiring()
{
  while (clearingDisks)
  {
    wait(10, msec);
  }
}

void AutoIntake::waitForIntake()
{
  while (intaking)
  {
    wait(10, msec);
  }
}

void AutoIntake::autonInit()
{
  fixingUnstable = false;
  clearingDisks = false;
  intaking = false;
  clearingLastDisk = false;
  lastCount = count;
}
