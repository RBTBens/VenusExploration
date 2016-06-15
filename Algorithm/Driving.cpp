#include "Definitions.h"
#include "Driving.h"

// Variables
Servo Driving::leftWheel;
Servo Driving::rightWheel;

// Base pulse trackers
volatile int Driving::leftPulses = 0;     // Number of pulses from the left encoder since start
volatile int Driving::rightPulses = 0;    // Number of pulses from the right encoder since start

// Positional variables
double Driving::relativeXPosition = 0;    // X distance in pulses relative to the begin position on the base
double Driving::relativeYPosition = 0;    // Y distance in pulses relative to the begin position on the base
double Driving::relativeOrientation = 0;  // Rotation in degrees relative to the robot facing out the base

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
#ifdef __DEBUG_DRIVING_TRIGGER
  Serial.print("Triggered on pin: ");
  Serial.println(pin);
#endif // __DEBUG_DRIVING

  // Add pulses accordingly
  if (pin == ID_LEFTENCODER)
    leftPulses++;
  else if (pin == ID_RIGHTENCODER)
    rightPulses++;
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

// Rotating function
void Driving::rotate(int dir)
{
#ifdef __DEBUG_DRIVING
  Serial.print("Action: rotate(");
  Serial.print(dir == 0 ? "random" : (dir > 0 ? "cw" : "ccw"));
  Serial.println(")");
#endif // __DEBUG_DRIVING

  // Rotate clockwise
  if (dir > 0)
  {
    leftWheel.write(LEFT_FORWARD);
    rightWheel.write(RIGHT_REVERSE);
  }
  // Rotate counter-clockwise
  else if (dir < 0)
  {
    leftWheel.write(LEFT_REVERSE);
    rightWheel.write(RIGHT_FORWARD);
  }
  // Rotate random direction
  else
  {
    rotate(random(2) == 0 ? 1 : -1);
  }
}

// Begins tracking pulses
PulseTracker Driving::startMeasurement(int needed)
{
  PulseTracker tracker(leftPulses, rightPulses, needed);
  return tracker;
}

// Public accessor of pulses
int Driving::getPulses()
{
  return (leftPulses + rightPulses) / 2;
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
  double* directionToBase = (double*)malloc(2 * sizeof(double));

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

// Blank initializer
PulseTracker::PulseTracker()
{
}

// Pulse tracker object constructor
PulseTracker::PulseTracker(int l, int r, int n)
{
  startLeft = l;
  startRight = r;
  neededPulses = n;
}

// Gets the pulses since start
int PulseTracker::getPulses()
{
  return Driving::getPulses() - (startLeft + startRight) / 2;
}

// Checks if the needed amount has been exceeded
bool PulseTracker::hasCompleted(int n)
{
  int num = n > 0 ? n : neededPulses;
  return getPulses() >= num;
}
