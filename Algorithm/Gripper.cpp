#include "Definitions.h"
#include "Gripper.h"

// Create Servo objects
Servo Gripper::gripperServo;

// Initialization function
void Gripper::initialize()
{
  // Attach the servo
  gripperServo.attach(ID_GRIPPERSERVO);
}

