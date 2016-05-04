// Global code identifiers
#define __DEBUG
//#define __ENCODERPRINT

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
  // Start up the engine
  engine.initialize();
}

// Loop function
void loop()
{
  engine.think();
}
