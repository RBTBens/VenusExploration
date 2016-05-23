// Includes
#include "Definitions.h"
#include "Driving.h"
#include "Gripper.h"
#include "UDS.h"
#include "IR.h"

// Create handler objects
Driving engine;
UDS sound;

// Variables
volatile byte portBHistory = 0xFF;  // Default is high because the pullups
volatile byte portDHistory = 0xFF;  // Default is high because the pullups

#ifdef __DEBUG
#include "DebugSerial.h"
DebugSerial debug;
#endif // __DEBUG

// Setup function
void setup()
{
  // Enable interrupts for the wheel encoders and the line sensors
  enableInterrupts();

#ifdef __DEBUG
  debug.open();
#endif // __DEBUG

  // Start up the engine
  engine.initialize();

  // Activate the UDS
  sound.initialize();
}

// Loop function
void loop()
{
#ifdef __DEBUG
  debug.read();
#endif // __DEBUG
}

// Interrupts
void enableInterrupts()
{
  // Switch interrupts off while messing whit them by clearing the global interrupt mask
  cli();
  
  // First set pins as input and enable pullups
  pinMode(ID_RIGHTLINESENSOR, INPUT);
  pinMode(ID_LEFTLINESENSOR, INPUT);
  pinMode(ID_LEFTENCODER, INPUT);
  pinMode(ID_RIGHTENCODER, INPUT);
  digitalWrite(ID_RIGHTLINESENSOR, HIGH);
  digitalWrite(ID_LEFTLINESENSOR, HIGH);
  digitalWrite(ID_LEFTENCODER, HIGH);
  digitalWrite(ID_RIGHTENCODER, HIGH);

  // Clear all existing interrupts
  PCIFR = B00000000;

  // Set PCIE0 and PCIE2 to enable PCMSK0 and PCMSK2 scan
  PCICR = B101;

  // Enable interrupts by setting the right bits in the Pin Change Mask Registers
  PCMSK0 = B00000001; // Enable PCINT0
  PCMSK2 = B10001100; // Enable PCINT23, PCINT19 and PCINT18

  // Turn interrupts back on by setting the global interrupt mask
  sei();
}

// Handle PCINT0 until PCINT7 interrupts
ISR(PCINT0_vect)
{
  byte changedBits;

  //XOR the current state of the B register with the previous state to look which pins have changed
  changedBits = PINB ^ portBHistory;
  portBHistory = PINB;


  // PCINT0 aka right encoder, has changed
  if(changedBits & (1 << PINB0))
  {
    engine.trigger(ID_RIGHTENCODER);
  }
}

// Handle PCINT16 until PCINT23 interrupts
ISR(PCINT2_vect)
{
  byte changedBits;

  //XOR the current state of the D register with the previous state to look which pins have changed
  changedBits = PIND ^ portDHistory;
  portBHistory = PIND;


  // PCINT18 aka right line sensor, has changed
  if(changedBits & (1 << PIND2))
  {
    
  }

  // PCINT19 aka left line sensor, has changed
  if(changedBits & (1 << PIND3))
  {
    
  }

  // PCINT23 aka left encoder, has changed
  if(changedBits & (1 << PIND7))
  {
    engine.trigger(ID_LEFTENCODER);
  }
}
