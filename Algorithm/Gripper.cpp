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

// Opens the gripper
void Gripper::open()
{
  gripperServo.write(GRIPPER_ANG_MIN);
}

// Closes the gripper
void Gripper::close()
{
  gripperServo.write(GRIPPER_ANG_MAX);
}

// Clap your hands
void Gripper::clapYourHands()
{
  for (int pos = 0; pos <= 110; pos++)
  {
    myservo.write(pos);
    delay(GRIPPER_CLAP_DELAY);
  }
  
  for (int pos = 110; pos >= 0; pos--)
  {
    myservo.write(pos);
    delay(GRIPPER_CLAP_DELAY);
  }
}
