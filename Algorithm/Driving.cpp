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

  // Attach interrupt for the encoders
  attachInterrupt(digitalPinToInterrupt(ID_LEFTENCODER), leftEncoderPulse, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ID_RIGHTENCODER), rightEncoderPulse, CHANGE);
}

// Interrupt left encoder
void Driving::leftEncoderPulse()
{
  // + 1 pulse left
#ifdef __DEBUG
  Serial.println("Left encoder pulse");
#endif
}

// Interrupt right encoder
void Driving::rightEncoderPulse()
{
  // + 1 pulse right
#ifdef __DEBUG
  Serial.println("Right encoder pulse");
#endif
}

