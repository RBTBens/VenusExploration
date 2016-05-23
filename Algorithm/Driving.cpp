#include "Definitions.h"
#include "Driving.h"

// Variables
Servo leftWheel;
Servo rightWheel;

// Set a null pointer on the instance
Driving* Driving::instance = NULL;

// Base pulse trackers
int nLeftPulses, nRightPulses;
int nRotatePulses;

int degreeToBase;
int pulsesToBase;

// Constructor
Driving::Driving()
{
  Driving::instance = this;
}

// Instance getter
Driving* Driving::getInstance()
{
  return Driving::instance;
}

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
    nLeftPulses++;
  else if (pin == ID_RIGHTENCODER)
    nRightPulses++;

  // Check rotation
  if (nRotatePulses > 0 && nLeftPulses >= nRotatePulses && nRightPulses >= nRotatePulses)
  {
    // End rotation
    drive(0);

    // Reset variables
    nRotatePulses = 0;
    nLeftPulses = 0;
    nRightPulses = 0;
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
  
  // Calculate needed pulses
  int neededPulses = round(abs(degree) / DEGREE_PER_PULSE);
  if (neededPulses < 1 && degree != 0)
    neededPulses = 1;
  
  // Apply to required pulses variables
  nRotatePulses = neededPulses;
  
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

#ifdef __DEBUG_DRIVING
  Serial.print("Action: drive(");
  Serial.print(dir == 0 ? "halt" : (dir > 0 ? "forward" : "reverse"));
  Serial.println(")");
#endif // __DEBUG_DRIVING
}

// Mapping function
void calculateNewPosition(int degreeTurned, int pulsesDriven)
{
  // Use polar coordinate addition to calculate the new position relative to the base;
}

// Mapping reset function
void resetPosition()
{
  degreeToBase = 0;
  pulsesToBase = 0;
}

