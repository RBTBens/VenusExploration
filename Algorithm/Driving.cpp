#include "Definitions.h"
#include "Driving.h"

// Create Servo objects
Servo leftWheel;
Servo rightWheel;

// Variables
int leftPulses = 0;
int rightPulses = 0;
bool leftState = false;
bool rightState = false;

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

void Driving::rotate(float degree)
{
#ifdef __DEBUG
  Serial.print("Action: rotate(");
  Serial.print(degree);
  Serial.println(" deg)");
#endif
  
  // Calculate needed pulses
  int neededPulses = round(abs(degree) / DEGREE_PER_PULSE);
  leftPulses = neededPulses;
  rightPulses = neededPulses;
  
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

// Basic driving function
void Driving::drive(int dir)
{
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
#endif
  
  // Set the pulses
  leftPulses = pulses;
  rightPulses = pulses;
  
  // Off you go!
  drive(dir);
}

// Interrupt left encoder
void Driving::leftEncoderPulse()
{
  if (leftPulses > 0)
    leftPulses--;
  else
    leftWheel.write(SERVO_NEUTRAL);
}

// Interrupt right encoder
void Driving::rightEncoderPulse()
{
  if (rightPulses > 0)
    rightPulses--;
  else
    rightWheel.write(SERVO_NEUTRAL);
}
