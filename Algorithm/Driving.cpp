#include "Definitions.h"
#include "Driving.h"

// Variables
Servo Driving::leftWheel;
Servo Driving::rightWheel;

// Base pulse trackers
volatile int Driving::leftPulses = 0;     // Number of pulses from the left encoder since the last instruction
volatile int Driving::rightPulses = 0;    // Number of pulses from the right encoder since the last instruction
volatile int Driving::rotatePulses = 0;   // Needed number of pulses to rotate
double Driving::relativeXPosition = 0;    // X distance in pulses relative to the begin position on the base
double Driving::relativeYPosition = 0;    // Y distance in pulses relative to the begin position on the base
double Driving::relativeOrientation = 0;  // Rotation in degrees relative to the robot facing out the base

// Callback
void (*Driving::rotateCallback)();

// Initialization function
void Driving::initialize()
{
  // Enable the encoders
  pinMode(ID_LEFTENCODER, INPUT);
  pinMode(ID_RIGHTENCODER, INPUT);
  
  // Attach the servos
  leftWheel.attach(ID_LEFTSERVO);
  rightWheel.attach(ID_RIGHTSERVO);

  // Turn the servos off
  drive(0);
}

// Interrupt callback
void Driving::trigger(byte pin)
{
  // Add pulses accordingly
  if (pin == ID_LEFTENCODER)
    leftPulses++;
  else if (pin == ID_RIGHTENCODER)
    rightPulses++;

  // Check rotation
  if (rotatePulses > 0 && leftPulses >= rotatePulses && rightPulses >= rotatePulses)
  {
    // End rotation
    drive(0);

    // Reset variables
    rotatePulses = 0;
    leftPulses = 0;
    rightPulses = 0;

    // Trigger callback
    if (rotateCallback != NULL)
      (*rotateCallback)();
  }
}

// Rotating function
void Driving::rotate(float degree, void (*callback)())
{
#ifdef __DEBUG_DRIVING
  Serial.print("Action: rotate(");
  Serial.print(degree);
  Serial.println(" deg)");
#endif // __DEBUG_DRIVING

  // Set the callback
  rotateCallback = callback;
  
  // Calculate needed pulses to rotate
  int neededPulses = round(abs(degree) / DEGREE_PER_PULSE);

  // Prevent robot from not rotating if the given degree is too small for one pulse
  if (neededPulses < 1 && degree != 0)
    neededPulses = 1;
  
  // Apply to required pulses variables
  rotatePulses = neededPulses;
  leftPulses = 0;
  rightPulses = 0;
  
  // Rotate clockwise
  if (degree > 0)
  {
    leftWheel.write(LEFT_FORWARD);
    rightWheel.write(RIGHT_REVERSE);
  }
  // Rotate counter-clockwise
  else if (degree < 0)
  {
    leftWheel.write(LEFT_REVERSE);
    rightWheel.write(RIGHT_FORWARD);
  }
}

// Driving function
void Driving::drive(int dir)
{
#ifdef __DEBUG_DRIVING
  Serial.print("Action: drive(");
  Serial.print(dir == 0 ? "halt" : (dir > 0 ? "forward" : "reverse"));
  Serial.println(")");
#endif // __DEBUG_DRIVING

  // Check the direction and change wheel power with it
  if (dir > 0)
  {
    leftWheel.write(LEFT_FORWARD);
    rightWheel.write(RIGHT_FORWARD);
  }
  else if (dir < 0)
  {
    leftWheel.write(LEFT_REVERSE);
    rightWheel.write(RIGHT_REVERSE);
  }
  else
  {
    leftWheel.write(SERVO_NEUTRAL);
    rightWheel.write(SERVO_NEUTRAL);
  }
}

// Mapping function
void Driving::calculateNewPosition(int degreeTurned, int pulsesDriven)
{
  // Convert the vector from polar to cartesian form
  double dX = pulsesDriven * cos(degreeTurned);
  double dY = pulsesDriven * sin(degreeTurned);

  // Calculate the new relative position to the base
  relativeXPosition += dX;
  relativeYPosition += dY;
  relativeOrientation += degreeTurned;

  // Make sure the orientation stays between -180 and 180 degrees
  while (relativeOrientation > 180)
    relativeOrientation -= 360;

  // Make sure the orientation stays between -180 and 180 degrees
  while (relativeOrientation < -180)
    relativeOrientation += 360;
}

// Calculate the direction to the base
double* Driving::calculateBaseDirection()
{
  double directionToBase[2];

  // Calculate the rotation needed to face the base
  directionToBase[0] = atan2(-relativeYPosition, -relativeXPosition);

  // Calulate the pulses needed to drive to the base
  directionToBase[1] = sqrt(sq(relativeXPosition) + sq(relativeYPosition));
  
  return directionToBase;
}

// Mapping reset function
void Driving::resetPosition()
{
  relativeXPosition = 0;
  relativeYPosition = 0;
  relativeOrientation = 180;  // When resetting the position the robot faces the container of the base to drop a sample, so it is rotated 180 degrees
}

