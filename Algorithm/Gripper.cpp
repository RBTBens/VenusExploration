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

// Set the gripper idle
void Gripper::idle()
{
  gripperServo.write(GRIPPER_ANG_IDLE);
}

// Clap your hands
void Gripper::clapYourHands()
{
  // Clap. One way, or another, I'm gonna find ya
  for (int pos = GRIPPER_CLAP_MIN; pos <= GRIPPER_ANG_MAX; pos++)
  {
    gripperServo.write(pos);
    delay(GRIPPER_CLAP_DELAY);
  }

  // I'm gonna get ya get ya get ya get ya
  for (int pos = GRIPPER_ANG_MAX; pos >= GRIPPER_CLAP_MIN; pos--)
  {
    gripperServo.write(pos);
    delay(GRIPPER_CLAP_DELAY);
  }
}
