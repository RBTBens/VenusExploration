#include "Definitions.h"
#include "Driving.h"

// Variables
Servo leftWheel;
Servo rightWheel;

// Encoders
bool bRotating, bFreedrive;
int nLeftPulses, nRightPulses;
byte nLeftPower, nRightPower;
unsigned long nLastLeftPulse, nLastRightPulse;
unsigned long nLeftPulseRate, nRightPulseRate;

// Positioning
float xPos, yPos;
float currentDeg;

// Constructor
Driving::Driving()
{
  // Set all basic variables
  bRotating = false;
  
  // Set the first pulse
  nLastLeftPulse = millis();
  nLastRightPulse = millis();
  nLeftPulseRate = 100;
  nRightPulseRate = 100;
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
    // Get pulse time
    unsigned long taken = millis() - nLastLeftPulse;
    nLastLeftPulse = millis();

    // Get the average pulse time
    if (taken < MAX_PULSE_TIME)
      nLeftPulseRate = ((nLeftPulseRate * 9) + taken) / 10;
    
#ifdef __ENCODER_INTERVALS
    Serial.print("Left pulse: [Since last: ");
    Serial.print(taken);
    Serial.print(" ms, Average: ");
    Serial.print(nLeftPulseRate);
    Serial.println(" ms]");
#endif // __ENCODER_INTERVALS

    // Check remaining pulses
    if (!bFreedrive && nLeftPulses > 0)
    {
      // Decrease by one
      nLeftPulses--;
      
      // Check if we're done
      if (nLeftPulses == 0)
      {
        leftWheel.write(SERVO_NEUTRAL);
  
        // Tell that we've stopped rotating
        if (bRotating && nRightPulses == 0)
          bRotating = false;
      }
    }
  }

  // Check if the left encoder has changed state
  else if (pin == ID_RIGHTENCODER)
  {
    // Get pulse time
    unsigned long taken = millis() - nLastRightPulse;
    nLastRightPulse = millis();

    // Get the average pulse time
    if (taken < MAX_PULSE_TIME)
      nRightPulseRate = ((nRightPulseRate * 9) + taken) / 10;
    
#ifdef __ENCODER_INTERVALS
    Serial.print("Right pulse: [Since last: ");
    Serial.print(taken);
    Serial.print(" ms, Average: ");
    Serial.print(nRightPulseRate);
    Serial.println(" ms]");
#endif // __ENCODER_INTERVALS

    // Check remaining pulses
    if (!bFreedrive && nRightPulses > 0)
    {
      // Decrease by one
      nRightPulses--;
      
      // Check if we're done
      if (nRightPulses == 0)
      {
        rightWheel.write(SERVO_NEUTRAL);
  
        // Tell that we've stopped rotating
        if (bRotating && nLeftPulses == 0)
          bRotating = false;
      }
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
  bRotating = true;
  
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
