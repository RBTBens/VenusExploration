#include "Definitions.h"
#include "Gripper.h"

// Create Servo objects
Servo gripperServo;

// Constructor
Gripper::Gripper()
{
  
}

// Initialization function
void Gripper::initialize()
{
  // Attach the servo
  gripperServo.attach(ID_GRIPPERSERVO);
}

