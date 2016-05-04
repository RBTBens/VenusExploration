#include "Arduino.h"
#include "UDS.h"

// Create Servo objects
Servo udsServo;

// Constructor
UDS::UDS()
{
  
}

// Destructor
UDS::~UDS()
{
  
}

// Initialization function
void UDS::initialize()
{
  // Attach the servo
  udsServo.attach(ID_UDSSERVO);
}