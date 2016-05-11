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
  Serial.println("Robot is alive!");
  Serial.println("Debugging options:");
  Serial.println("- a: rotate func [degrees, end with }]");
#endif

  // Start up the engine
  engine.initialize();
  
  // Attach the encoder interrupts
  engine.attachInterrupts(ISR_EncoderLeft, ISR_EncoderRight);
}

// Loop function
void loop()
{
#ifdef __DEBUG
  // Check if we have serial data
  if (Serial.available() > 0)
  {
    handleDebugCode(Serial.read());
  }
#endif
}

#ifdef __DEBUG
byte debugItem = 0;
byte writePointer = 0;
char debugStr[8];

void handleDebugCode(byte code)
{
  if (debugItem == 1)
  {
    if (code == '}')
    {
      int value = 0;
      for (int i = 0; i < writePointer; i++)
      {
        
      }
    }
    else
      debugStr[writePointer++] = code;
  }
  else if (debugItem == 0)
  {
    switch (code)
    {
      case 'a': debugItem = 1; writePointer = 0; break;
      default: break;
    }
  }
}
#endif

// Interrupt Service Routines
void ISR_EncoderLeft() { engine.leftEncoderPulse(); }
void ISR_EncoderRight() { engine.rightEncoderPulse(); }
