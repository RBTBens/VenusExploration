#include "Definitions.h"
#include "Line.h"

// Trigger function
void Line::trigger(byte pin)
{
  // Read the state of the sensors (active-low so take the inverse)
  bool right = !digitalRead(2);
  bool left = !digitalRead(3);

  // Get the current status from our Wireless comms
  RobotStatus robotStatus = (RobotStatus)Wireless::getVariable(VAR_STATUS);

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
      Driving::drive(-1, COLLISION_REVERSE_PULSES, onRightReverse, 111);
    }
  
    // Left line sensor triggered
    else if (!right && left)
    {
      Driving::drive(-1, COLLISION_REVERSE_PULSES, onLeftReverse, 112);
    }
  
    // Both line sensors triggered at once
    else if (right && left)
    {
      // Choose a random rotation direction
      long randomNumber = random(2);
      if (randomNumber == 0)
        Driving::drive(-1, COLLISION_REVERSE_PULSES, onRightReverse, 113);
      else
        Driving::drive(-1, COLLISION_REVERSE_PULSES, onLeftReverse, 114);
    }
  }
  // Stay between lines driving forward
  else if (robotStatus == DRIVING_UP_BASE)
  {
    // Right line sensor triggered
    if (right && !left)
    {
      // Rotate one pulse back and drive forward aggain
      Driving::rotate(-1 * DEGREE_PER_PULSE, -1, onRotateFinish, 204);
    }
    // Left line sensor triggered
    else if (!right && left)
    {
      // Rotate one pulse back and drive forward aggain
      Driving::rotate(1 * DEGREE_PER_PULSE, -1, onRotateFinish, 205);
    }
  }
  // Stay between lines driving backward
  else if (robotStatus == DRIVING_DOWN_BASE)
  {
    // Right line sensor triggered
    if (right && !left)
    {
      // Rotate one pulse back and drive forward aggain
      Driving::rotate(-1 * DEGREE_PER_PULSE, -1, onRotateFinishBackward, 206);
    }
    // Left line sensor triggered
    else if (!right && left)
    {
      // Rotate one pulse back and drive forward aggain
      Driving::rotate(1 * DEGREE_PER_PULSE, -1, onRotateFinishBackward, 207);
    }
  }
}

// Left callback
void Line::onLeftReverse()
{
  // Slightly random number of rotation pulses
  int rotationPulses = COLLISION_ROTATION_PULSES + random(COLLISION_RANDOM_ROTATION_PULSES);
  
  Driving::rotate(rotationPulses * DEGREE_PER_PULSE, -1, onRotateFinish, 208);
}

// Right callback
void Line::onRightReverse()
{
  // Slightly random number of rotation pulses
  int rotationPulses = -1 * COLLISION_ROTATION_PULSES - random(COLLISION_RANDOM_ROTATION_PULSES);
  
  Driving::rotate(rotationPulses * DEGREE_PER_PULSE, -1, onRotateFinish, 209);
}

// Rotate callback when driving forward
void Line::onRotateFinish()
{
  Serial.println("Line rotate finish");
  Driving::drive(1, 0, NULL, 115);
}

// Rotate callback when driving backward
void Line::onRotateFinishBackward()
{
  Serial.println("Line backward finish");
  Driving::drive(-1, 0, NULL, 116);
}
