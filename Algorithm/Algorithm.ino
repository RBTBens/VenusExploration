// Global code identifiers
#define __DEBUG

// Includes
#include "Driving.h"
#include "Gripper.h"
#include "UDS.h"
#include "IR.h"

// Create handler objects
Driving engine;

// Setup function
void setup()
{
#ifdef __DEBUG
  // Open Serial communications
  Serial.begin(9600);
#endif
  
  // Start up the engine
  engine.initialize();
}

// Loop function
void loop()
{
}
