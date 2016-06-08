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
RobotSubStatus subState = SUB_START;

// Setup function
void setup()
{
  // Enable interrupts for the wheel encoders and the line sensors
  enableInterrupts();

  // Take a seed from a not used analog pin to make the random values more random
  randomSeed(analogRead(A4));

  // Open talking to Serial
#ifdef __DEBUG_SERIAL
  DebugSerial::open();
#endif // __DEBUG_SERIAL

  // Open the wireless
  Wireless::open();

  // Start up the engine
  Driving::initialize();

  // Activate the UDS
  UDS::initialize();

  // Turn on the gripper
  Gripper::initialize();
}

// Rotate callback
void onRotatingFinish()
{
  subState = SUB_ROTATING_FINISH;
}

// Drive callback
void onDrivingFinish()
{
  subState = SUB_DRIVING_FINISH;
  Serial.println("Hey yo hoest mogelijk kerelman");
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
  RobotStatus robotStatus = (RobotStatus)Wireless::getVariable(VAR_STATUS, 0);
  RobotStatus otherRobotStatus = (RobotStatus)Wireless::getVariable(VAR_STATUS, 1);
  
  // Main algorithm switch
  switch (robotStatus)
  {
    case START_ON_BASE:
      if (subState == SUB_START)
      {
        // Close the gripper
        Gripper::idle();

        subState = SUB_DRIVING_COMMAND;
      }
      else if (subState == SUB_DRIVING_COMMAND)
      {
        // This robot is the last robot
        if (UDS::distanceAtDegree(UDS_ANGLE_BASE) < UDS_ROBOT_FRONT_DISTANCE)
        {
          // Wait x seconds to make sure the first robot is on its way
          delay(LAST_ROBOT_WAIT_TIME);

          // Set a callback to check if the robot is done driving forward; the last robot must drive further
          Driving::addCallback(LAST_OFF_BASE_PULSES, onDrivingFinish);
          
          // Drive the needed pulses forward to get off the base
          Driving::drive(1);
        }
        // This robot is the first robot
        else
        {
          // Set a callback to check if the robot is done driving forward; the last robot must drive further
          Driving::addCallback(FIRST_OFF_BASE_PULSES, onDrivingFinish);

          // Drive the needed pulses forward to get off the base
          Driving::drive(1);
        }

        subState = SUB_DRIVING;
      }
      else if (subState == SUB_DRIVING_FINISH)
      {
        // Done driving off the base, go to next state
        subState = SUB_ROTATING_COMMAND;
      }
      else if (subState == SUB_ROTATING_COMMAND)
      {
        // Set a callback to check if the robot is done rotating
        Driving::addCallback(-1, onRotatingFinish);
        
        // Rotate to drive to a random direction
        Driving::rotate(random(-5, 6) * DEGREE_PER_PULSE);

        subState = SUB_ROTATING;
      }
      else if (subState == SUB_ROTATING_FINISH)
      {
        // Go to the next state and search for a sample
        Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
        subState = SUB_DRIVING_COMMAND;
      }
      break;

    case SEARCHING_SAMPLE:
        if (subState == SUB_DRIVING_COMMAND)
        {
          Driving::drive(1);
          subState = SUB_DRIVING;
        }
        else if (subState == SUB_DRIVING)
        {        
          // Almost colliding with a mountain so rotate
          if (UDS::distanceAtDegree(UDS_ANGLE_BASE) < UDS_ROBOT_DISTANCE)
          {
            // Set a callback to check if the robot is done rotating
            Driving::addCallback(-1, onRotatingFinish);
            
            // Choose a random rotation direction
            long randomNumber = random(2);
            if (randomNumber == 0)
              Driving::rotate(6 * DEGREE_PER_PULSE);
            else
              Driving::rotate(-6 * DEGREE_PER_PULSE);

            subState = SUB_ROTATING;
          }
        }
        else if (subState == SUB_ROTATING_FINISH)
        { 
          subState = SUB_DRIVING_COMMAND;
        }        
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
        // Start clapping to celebrate this extraordinary presentation!
        Gripper::clapYourHands();
      else
      {
        // Stop driving     
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
