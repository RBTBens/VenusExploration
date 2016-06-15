#include "Definitions.h"
#include "Driving.h"

// Variables
Servo Driving::leftWheel;
Servo Driving::rightWheel;

// Base pulse trackers
volatile int Driving::leftPulses = 0;     // Number of pulses from the left encoder since start
volatile int Driving::rightPulses = 0;    // Number of pulses from the right encoder since start
volatile int Driving::forwardPulses = 0;  // Number of pulses from both encoders since start
volatile bool Driving::hadLeft = false;   // Whether or not we've had a left pulse for the rotate tracker
volatile bool Driving::hadRight = false;  // Whether or not we've had a left pulse for the rotate tracker

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
  drive(0, 0, NULL, 108);
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
  {
    leftPulses++;
    hadLeft = true;
  }
  else if (pin == ID_RIGHTENCODER)
  {
    rightPulses++;
    hadRight = true;
  }

  // Increase rotate pulses
  if (hadLeft && hadRight)
  {
    forwardPulses++;
    hadLeft = false;
    hadRight = false;
  }
}

// Rotating function
void Driving::rotate(float degree)
{
#ifdef __DEBUG_DRIVING
  Serial.print("Action: rotate(");
  Serial.print(degree);
  Serial.println(" deg)");
#endif // __DEBUG_DRIVING

  // Calculate needed pulses to rotate
  int neededPulses = round(abs(degree) / DEGREE_PER_PULSE);

  // Prevent robot from not rotating if the given degree is too small for one pulse
  if (neededPulses < 1 && degree != 0)
    neededPulses = 1;

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

// Begins tracking pulses
PulseTracker Driving::startMeasurement(int needed)
{
  PulseTracker tracker(forwardPulses, needed);
  return tracker;
}

// Public accessor of pulses
int Driving::getPulses()
{
  return forwardPulses;
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
PulseTracker::PulseTracker(int p, int n)
{
  startPulses = p;
  neededPulses = n;
}

// Gets the pulses since start
int PulseTracker::getPulses()
{
  return Driving::getPulses() - startPulses;
}

// Checks if the needed amount has been exceeded
bool PulseTracker::hasCompleted(int n)
{
  int num = n > 0 ? n : neededPulses;
  return getPulses() >= num;
}
