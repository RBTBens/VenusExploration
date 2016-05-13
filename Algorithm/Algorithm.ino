// Includes
#include "Definitions.h"
#include "Driving.h"
#include "Gripper.h"
#include "UDS.h"
#include "IR.h"

// Create handler objects
Driving engine;

#ifdef __DEBUG
#include "DebugSerial.h"
DebugSerial debug(engine);
#endif // __DEBUG

// Setup function
void setup()
{
#ifdef __DEBUG
  debug.open();
#endif // __DEBUG

  // Start up the engine
  engine.initialize();
}

// Loop function
void loop()
{
#ifdef __DEBUG
  debug.read();
#endif // __DEBUG

  // Run engine logic
  engine.loop();
}
