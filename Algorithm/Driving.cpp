#include "Arduino.h"
#include "Driving.h"
#include <Servo.h>

// Create Servo objects
Servo leftWheel;
Servo rightWheel;

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


void Driving::rotate(short degree)
{
  
}

void Driving::drive(byte dir)
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


// Add an interrupt
void Driving::attachInterrupts(void (*leftFunc)(), void (*rightFunc)())
{
  attachInterrupt(digitalPinToInterrupt(ID_LEFTENCODER), *leftFunc, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ID_RIGHTENCODER), *rightFunc, CHANGE);
}

// Interrupt left encoder
void Driving::leftEncoderPulse()
{
#ifdef __DEBUG
  Serial.println("Left encoder pulse");
#endif
}

// Interrupt right encoder
void Driving::rightEncoderPulse()
{
#ifdef __DEBUG
  Serial.println("Right encoder pulse");
#endif
}
