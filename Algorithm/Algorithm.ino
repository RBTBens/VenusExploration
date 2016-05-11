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
#endif

// Setup function
void setup()
{
#ifdef __DEBUG
  // Open Serial communications
  Serial.begin(9600);
  Serial.println("Robot is alive!");

  debug.open();
#endif

  // Start up the engine
  engine.initialize();
}

// Loop function
void loop()
{
#ifdef __DEBUG
  // Check if we have serial data
  if (Serial.available() > 0)
  {
    debug.handle(Serial.read());
  }
#endif

  // Run engine logic
  engine.loop();
}
