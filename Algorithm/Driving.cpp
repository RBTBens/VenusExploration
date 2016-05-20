#include "Definitions.h"
#include "Driving.h"

// Variables
Servo leftWheel;
Servo rightWheel;

// Encoders
bool bFreedrive;
int nLeftPulses, nRightPulses;
byte nLeftPower, nRightPower;

// Constructor
Driving::Driving()
{
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
void Driving::trigger(byte pin, bool state)
{
  // Check if the left encoder has changed state
  if (pin == ID_LEFTENCODER)
  {
    // Check remaining pulses
    if (!bFreedrive && nLeftPulses > 0)
    {
      // Decrease by one
      nLeftPulses--;
      
      // Check if we're done
      if (nLeftPulses == 0)
        leftWheel.write(SERVO_NEUTRAL);
    }
  }

  // Check if the left encoder has changed state
  else if (pin == ID_RIGHTENCODER)
  {
    // Check remaining pulses
    if (!bFreedrive && nRightPulses > 0)
    {
      // Decrease by one
      nRightPulses--;
      
      // Check if we're done
      if (nRightPulses == 0)
        rightWheel.write(SERVO_NEUTRAL);
    }
  }
}

// Rotating function
void Driving::rotate(float degree)
{
#ifdef __DEBUG
  Serial.print("Action: rotate(");
  Serial.print(degree);
  Serial.println(" deg)");
#endif // __DEBUG
  
  // Calculate needed pulses
  int neededPulses = round(abs(degree) / DEGREE_PER_PULSE);
  if (neededPulses < 1 && degree != 0)
    neededPulses = 1;

  // Apply to required pulses variables
  nLeftPulses = neededPulses;
  nRightPulses = neededPulses;
  
  // Rotate clockwise
  if (degree > 0)
  {
    nLeftPower = LEFT_FORWARD;
    nRightPower = RIGHT_REVERSE;
  }
  // Rotate counter-clockwise
  else if (degree < 0)
  {
    nLeftPower = LEFT_REVERSE;
    nRightPower = RIGHT_FORWARD;
  }

  // Apply to the servos
  leftWheel.write(nLeftPower);
  rightWheel.write(nRightPower);
}

// Basic driving function
void Driving::drive(int dir)
{
  if (dir > 0)
  {
    nLeftPower = LEFT_FORWARD;
    nRightPower = RIGHT_FORWARD;
  }
  else if (dir < 0)
  {
    nLeftPower = LEFT_REVERSE;
    nRightPower = RIGHT_REVERSE;
  }
  else
  {
    nLeftPower = SERVO_NEUTRAL;
    nRightPower = SERVO_NEUTRAL;
  }
  
  // Apply to the servos
  leftWheel.write(nLeftPower);
  rightWheel.write(nRightPower);
}

// Extended driving function
void Driving::drive(int dir, int pulses)
{
  // Check if we want to reverse with pulses
  if (pulses < 0)
  {
    dir = -1;
    pulses = abs(pulses);
  }
  
#ifdef __DEBUG
  Serial.print("Action: drive(");
  Serial.print(dir > 0 ? "forward" : "reverse");
  Serial.print(",");
  Serial.print(pulses);
  Serial.println(" pulses)");
#endif // __DEBUG
  
  // Set the pulses
  nLeftPulses = pulses;
  nRightPulses = pulses;

  // Set freedrive boolean
  bFreedrive = pulses == 0;
  
  // Off you go!
  drive(dir);
}
