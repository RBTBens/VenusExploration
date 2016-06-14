#include "Definitions.h"
#include "Driving.h"

// Variables
Servo Driving::leftWheel;
Servo Driving::rightWheel;

// Base pulse trackers
volatile int Driving::leftPulses = 0;       // Number of pulses from the left encoder since the last instruction
volatile int Driving::rightPulses = 0;      // Number of pulses from the right encoder since the last instruction
volatile int Driving::rotatePulses = 0;     // Needed number of pulses to rotate
double Driving::relativeXPosition = 0;      // X distance in pulses relative to the begin position on the base
double Driving::relativeYPosition = 0;      // Y distance in pulses relative to the begin position on the base
double Driving::relativeOrientation = 180;  // Rotation in degrees relative to the robot facing the base (starts driving out the base, so 180)

// Callback variables
int Driving::callbackPulses = 0;
void (*Driving::callbackFunc)() = NULL;

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

  // Check rotation
  if (abs(rotatePulses) > 0 && leftPulses >= abs(rotatePulses) && rightPulses >= abs(rotatePulses))
  {
    // End rotation
    drive(0);
    
    // Trigger callback (callbackPulses must be negative for a rotation callback instead of a driving callback)
    if (callbackPulses < 0)
      Serial.println("Run callback from rotation trigger");
      runCallback();
  }
  // Check driving
  else if (rotatePulses == 0 && leftPulses >= callbackPulses && rightPulses >= callbackPulses)
  {
    // End driving
    drive(0);
    
    // Trigger callback
    Serial.println("Run callback from driving trigger");
    runCallback();
  }
}

// Rotating function
void Driving::rotate(double degree)
{
#ifdef __DEBUG_DRIVING
  Serial.print("Action: rotate(");
  Serial.print(degree);
  Serial.println(" deg)");
#endif // __DEBUG_DRIVING

  // Calculate needed pulses to rotate
  int neededPulses = round(degree / DEGREE_PER_PULSE);

  // Map the previous movement and reset the pulses variables
  mapAndReset();
  
  // Apply to required pulses
  rotatePulses = neededPulses;
  
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

  // Map the previous movement and reset the pulses variables
  mapAndReset();

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

// Callback adding function
void Driving::addCallback(int pulses, void (*callback)())
{
  // Run previous callback
  runCallback();
  
  // Set the callback
  callbackFunc = callback;
  callbackPulses = pulses;
}

// Run callback with reset
void Driving::runCallback()
{
  Serial.println("Run callback jonguh");
  // Temporarily store the pointer
  void (*callback)() = callbackFunc;
  if (callback == NULL)
    return;

  // Reset values
  callbackFunc = NULL;
  callbackPulses = 0;

  // Trigger it
  (*callback)();
}

// Function to map the previous movement and reset the pulses variables
void Driving::mapAndReset()
{
  int averagePulses = round((leftPulses + rightPulses) / 2);
  
  // Map rotation clockwise
  if (rotatePulses > 0)
  {
    calculateNewPosition(averagePulses * DEGREE_PER_PULSE, 0);
  }
  // Map rotation counter-clockwise
  else if(rotatePulses < 0)
  {
    calculateNewPosition(-averagePulses * DEGREE_PER_PULSE, 0);
  }
  // Map driving
  else
  {
    calculateNewPosition(0, averagePulses);
  }
  
  // Reset pulses variables
  rotatePulses = 0;
  leftPulses = 0;
  rightPulses = 0;
}

// Function to calculate the new relative position
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

#ifdef __DEBUG_DRIVING
  Serial.print("relativeXPosition: ");
  Serial.println(relativeXPosition);
  Serial.print("relativeYPosition: ");
  Serial.println(relativeYPosition);
  Serial.print("relativeOrientation: ");
  Serial.println(relativeOrientation);
#endif // __DEBUG_DRIVING
}

// Calculate the direction to the base
double* Driving::calculateBaseDirection()
{
  double* directionToBase = (double*)malloc(2 * sizeof(double));

  // Calculate the rotation needed to face the base
  double rotationToBase = (atan2(-relativeYPosition, -relativeXPosition) / M_PI) * 180 - relativeOrientation;

  // Make sure the rotation stays between -180 and 180 degrees
  while (rotationToBase < -180)
    rotationToBase += 360;

  while (rotationToBase > 180)
    rotationToBase -= 360;
  
  directionToBase[0] = rotationToBase;

  // Calulate the pulses needed to drive to the base
  directionToBase[1] = sqrt(sq(relativeXPosition) + sq(relativeYPosition));
  
  return directionToBase;
}

// Mapping reset function
void Driving::resetPosition()
{
  relativeXPosition = 0;
  relativeYPosition = 0;
  relativeOrientation = 0;
}

