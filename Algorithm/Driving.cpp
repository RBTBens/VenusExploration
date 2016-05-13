#include "Definitions.h"
#include "Driving.h"

// Create Servo objects
Servo leftWheel, rightWheel;

// Pulse trackers
int leftPulses, rightPulses;
bool leftState, rightState, rotating;
byte leftPower, rightPower;
unsigned long leftLastPulse, rightLastPulse;

// Positional variables
float xPos, yPos;
float currentDeg;

// Constructor
Driving::Driving()
{
  // Set the first pulse
  leftLastPulse = millis();
  rightLastPulse = millis();
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

// Loop function
void Driving::loop()
{
  // Get the current states
  bool newLeftState = digitalRead(ID_LEFTENCODER) == HIGH;
  bool newRightState = digitalRead(ID_RIGHTENCODER) == HIGH;

  // Check if the left encoder has changed state
  if (newLeftState != leftState)
  {
    leftState = newLeftState;
    leftEncoderPulse();
  }

  // Check if the right encoder has changed state
  if (newRightState != rightState)
  {
    rightState = newRightState;
    rightEncoderPulse();
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
  leftPulses = neededPulses;
  rightPulses = neededPulses;
  rotating = true;
  
  // Rotate clockwise
  if (degree > 0)
  {
    leftPower = LEFT_FORWARD;
    rightPower = RIGHT_REVERSE;
  }
  // Rotate counter-clockwise
  else if (degree < 0)
  {
    leftPower = LEFT_REVERSE;
    rightPower = RIGHT_FORWARD;
  }

  // Apply to the servos
  leftWheel.write(leftPower);
  rightWheel.write(rightPower);
}

// Basic driving function
void Driving::drive(int dir)
{
  if (dir > 0)
  {
    leftPower = LEFT_FORWARD;
    rightPower = RIGHT_FORWARD;
  }
  else if (dir < 0)
  {
    leftPower = LEFT_REVERSE;
    rightPower = RIGHT_REVERSE;
  }
  else
  {
    leftPower = SERVO_NEUTRAL;
    rightPower = SERVO_NEUTRAL;
  }
  
  // Apply to the servos
  leftWheel.write(leftPower);
  rightWheel.write(rightPower);
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
  leftPulses = pulses;
  rightPulses = pulses;
  
  // Off you go!
  drive(dir);
}

// Power adjusting (Temporary I think)
void Driving::setPower(int value)
{
  rightPower = value;
  rightWheel.write(rightPower);
}

// Adjust wheel rotation rate
byte Driving::adjustWheelRate(int pulses, byte input)
{
  if (rotating || pulses > DRIVE_BRAKE_PULSES)
    return input;
  else
  {
    Serial.println("I want to BRAKE free!");

    // To-Do: Change this to work and move it outside of the pulses
    
    int dir = SERVO_NEUTRAL - input;
    dir = dir / abs(dir);

    float part = (DRIVE_BRAKE_PULSES - pulses) / DRIVE_BRAKE_PULSES;
    float frac = part * SERVO_DIFFERENCE; // 0/5 until 5/5 * 16

    Serial.print("Input ");
    Serial.print(input);
    Serial.print(" Dir ");
    Serial.print(dir);
    Serial.print(" Frac ");
    Serial.print(frac);
    Serial.print(" |-> ");
    Serial.print(input + dir * frac);
    Serial.println();
    
    return round(input + dir * frac);
  }
}

// Interrupt left encoder
void Driving::leftEncoderPulse()
{
#ifdef __ENCODER_INTERVALS
  unsigned long now = millis();
  unsigned long taken = now - leftLastPulse;
  leftLastPulse = now;

  Serial.print("Left pulse: ");
  Serial.print(taken);
  Serial.println(" sec");
#endif // __ENCODER_INTERVALS

  // Check remaining pulses
  if (leftPulses > 0)
  {
    // Decrease
    leftPulses--;

    // Check if we're done
    if (leftPulses == 0)
    {
      leftWheel.write(SERVO_NEUTRAL);

      // Tell that we've stopped rotating
      if (rotating && rightPulses == 0)
        rotating = false;
    }
    else
    {
      byte rate = adjustWheelRate(leftPulses, leftPower);
      if (rate != leftPower)
      {
        leftPower = rate;
        leftWheel.write(leftPower);
      }
    }
  }
}

// Interrupt right encoder
void Driving::rightEncoderPulse()
{
#ifdef __ENCODER_INTERVALS
  unsigned long now = millis();
  unsigned long taken = now - rightLastPulse;
  rightLastPulse = now;

  Serial.print("Right pulse: ");
  Serial.print(taken);
  Serial.println(" sec");
#endif // __ENCODER_INTERVALS

  // Check remaining pulses
  if (rightPulses > 0)
  {
    // Decrease
    rightPulses--;

    // Check if we're done
    if (rightPulses == 0)
    {
      rightWheel.write(SERVO_NEUTRAL);

      // Tell that we've stopped rotating
      if (rotating && leftPulses == 0)
        rotating = false;
    }
    else
    {
      byte rate = adjustWheelRate(rightPulses, rightPower);
      if (rate != rightPower)
      {
        rightPower = rate;
        rightWheel.write(rightPower);
      }
    }
  }
}
