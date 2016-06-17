#include "Definitions.h"
#include "Driving.h"

// Variables
Servo Driving::leftWheel;
Servo Driving::rightWheel;

// Base pulse trackers
volatile int Driving::leftPulses = 0;                     // Number of pulses from the left encoder since start
volatile int Driving::rightPulses = 0;                    // Number of pulses from the right encoder since start

// Positional variables
double Driving::relativeXPosition = 0;                    // X distance in pulses relative to the begin position on the base
double Driving::relativeYPosition = 0;                    // Y distance in pulses relative to the begin position on the base
double Driving::relativeOrientation = 180;                // Rotation in degrees relative to the robot facing the base (starts driving out the base, so 180)

// Mapping variables
DrivingStatus Driving::drivingStatus = DRIVING_NEUTRAL;   // Enum to keep track of the driving status for the position mapping
int Driving::lastLeftPulses = 0;                          // Number of pulses from the left encoder at last mapping
int Driving::lastRightPulses = 0;                         // Number of pulses from the right encoder at last mapping

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

  // Map the previous move before the state changes and the servos change direction
  map();

  // Check the direction and change wheel power with it
  if (dir > 0)
  {
    leftWheel.write(LEFT_FORWARD);
    rightWheel.write(RIGHT_FORWARD);
    drivingStatus = DRIVING_FORWARD;
  }
  else if (dir < 0)
  {
    leftWheel.write(LEFT_REVERSE);
    rightWheel.write(RIGHT_REVERSE);
    drivingStatus = DRIVING_REVERSE;
  }
  else
  {
    leftWheel.write(SERVO_NEUTRAL);
    rightWheel.write(SERVO_NEUTRAL);
    drivingStatus = DRIVING_NEUTRAL;
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

  // Map the previous move before the state changes and the servos change direction
  map();

  // Rotate clockwise
  if (dir > 0)
  {
    leftWheel.write(LEFT_FORWARD);
    rightWheel.write(RIGHT_REVERSE);
    drivingStatus = DRIVING_ROTATING_CW;
  }
  // Rotate counter-clockwise
  else if (dir < 0)
  {
    leftWheel.write(LEFT_REVERSE);
    rightWheel.write(RIGHT_FORWARD);
    drivingStatus = DRIVING_ROTATING_CCW;
  }
  // Rotate random direction
  else
  {
    // Call this function aggaing with dir 1 or -1
    rotate(random(2) == 0 ? 1 : -1);
  }
}

/*
 * Pulse Tracking
 */

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
  // If we've reset, return false
  if (neededPulses < 0)
    return false;
  
  // Check with the given parameter or with the earlier specified amount of needed pulses if 0
  int num = n > 0 ? n : neededPulses;
  return getPulses() >= num;
}

// Resets the tracker object by negating neededPulses
void PulseTracker::reset()
{
  neededPulses = -1;
}

/*
 * Mapping
 */

// Function to map the previous move of the robot
void Driving::map()
{
  // Calculate the average driven pulses of both wheels since the last map
  int averageDrivenPulses = ((leftPulses - lastLeftPulses) + (rightPulses - lastRightPulses)) / 2;
  
  // Store there current value for the next map
  lastLeftPulses = leftPulses;
  lastRightPulses = rightPulses;

  // Check which status we're in
  switch (drivingStatus)
  {
    case DRIVING_FORWARD:
      calculateNewPosition(0, averageDrivenPulses);
      break;

    case DRIVING_REVERSE:
      calculateNewPosition(0, -averageDrivenPulses);
      break;

    case DRIVING_ROTATING_CW:
      calculateNewPosition(averageDrivenPulses * DEGREE_PER_PULSE, 0);
      break;

    case DRIVING_ROTATING_CCW:
      calculateNewPosition(-averageDrivenPulses * DEGREE_PER_PULSE, 0);
      break;
  }
}

// Function to calculate the new position of the robot
void Driving::calculateNewPosition(double degreeTurned, double pulsesDriven)
{
  // Convert the vector from polar to cartesian form
  double dX = pulsesDriven * cos((relativeOrientation / 180) * M_PI);
  double dY = pulsesDriven * sin((relativeOrientation / 180) * M_PI);

  // Calculate the new relative position to the base
  relativeXPosition += dX;
  relativeYPosition += dY;
  relativeOrientation += degreeTurned;

  // Make sure the orientation stays between -180 and 180 degrees
  while (relativeOrientation < -180)
    relativeOrientation += 360;
  while (relativeOrientation > 180)
    relativeOrientation -= 360;

#ifdef __DEBUG_DRIVING_POSITION
  Serial.print("relativeXPosition: ");
  Serial.println(relativeXPosition);
  Serial.print("relativeYPosition: ");
  Serial.println(relativeYPosition);
  Serial.print("relativeOrientation: ");
  Serial.println(relativeOrientation);
#endif // __DEBUG_DRIVING_POSITION
}

// Calculate the direction to the base
double* Driving::calculateBaseDirection()
{
  // First allocate two doubles in memory
  double* directionToBase = (double*)malloc(2 * sizeof(double));

  // Calculate the rotation needed to face the base
  double rotationToBase = (atan2(-relativeYPosition, -relativeXPosition) / M_PI) * 180 - relativeOrientation;

  // Make sure the rotation stays between -180 and 180 degrees
  while (rotationToBase < -180)
    rotationToBase += 360;
  while (rotationToBase > 180)
    rotationToBase -= 360;

  // Calulate the pulses needed to drive to the base
  directionToBase[0] = rotationToBase;
  directionToBase[1] = sqrt(sq(relativeXPosition) + sq(relativeYPosition));
  
  return directionToBase;
}

// Mapping reset function driving out the base
void Driving::resetPositionForward()
{
  relativeXPosition = 0;
  relativeYPosition = 0;
  relativeOrientation = 180;
}

// Mapping reset function driving in the base
void Driving::resetPositionReverse()
{
  relativeXPosition = 0;
  relativeYPosition = 0;
  relativeOrientation = 0;
}
