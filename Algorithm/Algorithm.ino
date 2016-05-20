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
  // Enable interrupts
  enableInterrupt(7);
  enableInterrupt(8);

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
}

// Interrupts
void enableInterrupt(byte pin)
{
  // First write pullup
  digitalWrite(pin, HIGH);

  // Set the pointer to work
  *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));

  // Clear all existing interrupts
  PCIFR |= bit(digitalPinToPCICRbit(pin));

  // And enable them again
  PCICR |= bit(digitalPinToPCICRbit(pin));
}

// Handle D8 until D13 interrupts
ISR(PCINT0_vect)
{
  engine.trigger(8);
}

// Handle D0 until D7 interrupts
ISR(PCINT2_vect)
{
  engine.trigger(7);
}  

