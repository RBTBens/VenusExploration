// Main includes
#include "Definitions.h"
#include "Driving.h"
#include "Gripper.h"
#include "UDS.h"
#include "Line.h"

// Debug includes
#ifdef __DEBUG_SERIAL
#include "DebugSerial.h"
#endif // __DEBUG_SERIAL

// Variables
int subState = 0;

// Setup function
void setup()
{
  // Enable interrupts for the wheel encoders and the line sensors
  enableInterrupts();

  // Open talking to Serial
#ifdef __DEBUG_SERIAL
  DebugSerial::open();
#else // __DEBUG_SERIAL
  Wireless::open();
#endif // __DEBUG_SERIAL

  // Start up the engine
  Driving::initialize();

  // Activate the UDS
  UDS::initialize();

  // Turn on the gripper
  Gripper::initialize();
}

// Loop function
void loop()
{
  // Read input from Serial command
#ifdef __DEBUG_SERIAL
  DebugSerial::read();
#else // __DEBUG_SERIAL
  Wireless::read();
#endif // __DEBUG_SERIAL

  // Get the current status from our Wireless comms
  RobotStatus robotStatus = (RobotStatus)Wireless::getVariable(VAR_STATUS);
  RobotStatus otherRobotStatus = (RobotStatus)Wireless::getVariable(VAR_STATUS);
  
  // Main algorithm switch
  switch (robotStatus)
  {
    case START_ON_BASE:
      // Close the gripper
      Gripper::close();
      
      // Go to the next state and search for a sample
      Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
      subState = SUB_DRIVING_COMMAND;
      break;

    case SEARCHING_SAMPLE:
        if (subState == SUB_DRIVING_COMMAND)
        {
          Driving::drive(1);
          subState = SUB_DRIVING;
        }
        else if (subState == SUB_DRIVING)
        {
          
        }

//      // Constant sweeping
//        if (currentDegree <= UDS_SWEEP_MIN)
//          sweepDirection = 1;
//        else if (currentDegree >= UDS_SWEEP_MAX)
//          sweepDirection = -1;
//        
//        // Do a measurement every 5 degrees
//        if (currentDegree % 5 == 0)
//        {
// Change value and apply to the servo
//  currentDegree += sweepDirection;
//  udsServo.write(currentDegree);
        
      break;

    case PICKING_UP_SAMPLE:
      //
      break;

    case SEARCHING_BASE:
      //
      break;

    case DRIVING_UP_BASE:
      //
      break;

    case DROPPING_SAMPLE:
      //
      break;

    case DRIVING_DOWN_BASE:
      //
      break;

    case DONE:
      if (subState == SUB_CLAP)
        Gripper::clapYourHands();
      else
      {     
        Driving::drive(0);
        subState = SUB_CLAP;
      }
      break;
  }
}

// Hystory of the pin registers
volatile byte portBHistory = 0xFF;  // Default is high because the pullups
volatile byte portDHistory = 0xFF;  // Default is high because the pullups

// Enable the interrupts
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
  //XOR the current state of the B register with the previous state to look which pins have changed
  byte changedBits = PINB ^ portBHistory;
  portBHistory = PINB;
  
  // PCINT0 aka right encoder, has changed
  if (changedBits & (1 << PINB0))
  {
    Driving::trigger(ID_RIGHTENCODER);
  }
}

// Handle PCINT16 until PCINT23 interrupts
ISR(PCINT2_vect)
{
  //XOR the current state of the D register with the previous state to look which pins have changed
  byte changedBits = PIND ^ portDHistory;
  portDHistory = PIND;
  
  // PCINT18 aka right line sensor, has changed
  if (changedBits & (1 << PIND2))
  {
    Line::trigger(ID_RIGHTLINESENSOR);
  }

  // PCINT19 aka left line sensor, has changed
  if (changedBits & (1 << PIND3))
  {
    Line::trigger(ID_LEFTLINESENSOR);
  }

  // PCINT23 aka left encoder, has changed
  if (changedBits & (1 << PIND7))
  {
    Driving::trigger(ID_LEFTENCODER);
  }
}
