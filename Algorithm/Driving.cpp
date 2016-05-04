#include "Arduino.h"
#include "Driving.h"

// Create Servo objects
Servo leftWheel;
Servo rightWheel;

// Constructor
Driving::Driving()
{
  
}

// Destructor
Driving::~Driving()
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

// Think function
void Driving::think()
{
  
}
