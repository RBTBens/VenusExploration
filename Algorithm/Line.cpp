#include "Definitions.h"
#include "Line.h"

// Trigger function
void Line::trigger(byte pin)
{
  // Read the state of the sensors (active-low so take the inverse)
  bool right = !digitalRead(2);
  bool left = !digitalRead(3);

  // Get the current status from our Wireless comms
  RobotStatus robotStatus = (RobotStatus)Wireless::getVariable(VAR_STATUS, 0);

#ifdef __DEBUG_LINE
  Serial.print("Triggered on pin: ");
  Serial.println(pin);
  Serial.print("Right(2): ");
  Serial.println(right);
  Serial.print("Left(3): ");
  Serial.println(left);
#endif // __DEBUG_LINE

  // Collision detection
  if (robotStatus == SEARCHING_SAMPLE || robotStatus == SEARCHING_BASE)
  {
    // Right line sensor triggered
    if (right && !left)
    {
      Driving::addCallback(COLLISION_REVERSE_PULSES, onRightReverse);
      Driving::drive(-1);
    }
  
    // Left line sensor triggered
    else if (!right && left)
    {
      Driving::addCallback(COLLISION_REVERSE_PULSES, onLeftReverse);
      Driving::drive(-1);
    }
  
    // Both line sensors triggered at once
    else if (right && left)
    {
      // Choose a random rotation direction
      long randomNumber = random(2);
      if (randomNumber == 0)
        Driving::addCallback(COLLISION_REVERSE_PULSES, onRightReverse);
      else
        Driving::addCallback(COLLISION_REVERSE_PULSES, onLeftReverse);
      Driving::drive(-1);
    }
  }
  // Stay between lines driving forward
  else if (robotStatus == DRIVING_UP_BASE)
  {
    // Right line sensor triggered
    if (right && !left)
    {
      // Rotate one pulse back and drive forward aggain
      Driving::addCallback(-1, onRotateFinish);
      Driving::rotate(-1 * DEGREE_PER_PULSE);
    }
    // Left line sensor triggered
    else if (!right && left)
    {
      // Rotate one pulse back and drive forward aggain
      Driving::addCallback(-1, onRotateFinish);
      Driving::rotate(1 * DEGREE_PER_PULSE);
    }
  }
  // Stay between lines driving backward
  else if (robotStatus == DRIVING_DOWN_BASE)
  {
    // Right line sensor triggered
    if (right && !left)
    {
      // Rotate one pulse back and drive forward aggain
      Driving::addCallback(-1, onRotateFinishBackward);
      Driving::rotate(-1 * DEGREE_PER_PULSE);
    }
    // Left line sensor triggered
    else if (!right && left)
    {
      // Rotate one pulse back and drive forward aggain
      Driving::addCallback(-1, onRotateFinishBackward);
      Driving::rotate(1 * DEGREE_PER_PULSE);
    }
  }
}

// Left callback
void Line::onLeftReverse()
{
  // Slightly random number of rotation pulses
  int rotationPulses = COLLISION_ROTATION_PULSES + random(COLLISION_RANDOM_ROTATION_PULSES);
  
  Driving::addCallback(-1, onRotateFinish);
  Driving::rotate(rotationPulses * DEGREE_PER_PULSE);
}

// Right callback
void Line::onRightReverse()
{
  // Slightly random number of rotation pulses
  int rotationPulses = -1 * COLLISION_ROTATION_PULSES - random(COLLISION_RANDOM_ROTATION_PULSES);
  
  Driving::addCallback(-1, onRotateFinish);
  Driving::rotate(rotationPulses * DEGREE_PER_PULSE);
}

// Rotate callback when driving forward
void Line::onRotateFinish()
{
  Driving::drive(1);
}

// Rotate callback when driving backward
void Line::onRotateFinishBackward()
{
  Driving::drive(-1);
}
