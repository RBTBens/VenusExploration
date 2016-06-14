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
int UDSDegree = UDS_SWEEP_MIN;
bool UDSSweepDirection = 0;

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
  Serial.println("Finished rotating!");
}

// Drive callback
void onDrivingFinish()
{
  subState = SUB_DRIVING_FINISH;
  Serial.println("Finished driving!");
}

// Get the robot substate into the debug
RobotSubStatus DebugSerial::getSubStatus()
{
  return subState;
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
  RobotStatus otherRobotStatus = (RobotStatus)Wireless::getVariable(VAR_STATUS, true);
  
  // Main algorithm switch
  switch (robotStatus)
  {
    case START_ON_BASE:
    {
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
    }
      break;

    case SEARCHING_SAMPLE:
    {
        if (subState == SUB_DRIVING_COMMAND)
        {
          Driving::drive(1);
          subState = SUB_DRIVING;
        }
        else if (subState == SUB_DRIVING)
        {
          // Make the UDS sweep
          if (UDSSweepDirection == 0)
          {
            if (UDSDegree < UDS_SWEEP_MAX)
              UDSDegree++;
            else
            {
              UDSDegree = UDS_SWEEP_MIN;
              UDSSweepDirection = 1;
            }
          }
          else
          {
            if (UDSDegree > UDS_SWEEP_MIN)
              UDSDegree--;
            else
            {
              UDSDegree = UDS_SWEEP_MAX;
              UDSSweepDirection = 0;
            }
          }
                  
          // Almost colliding with a mountain so rotate
          if (UDS::distanceAtDegree(UDSDegree, true) < UDS_COLLISION_DISTANCE)
          {
            subState = SUB_ROTATING_COMMAND;           
          }

          // Read the IR sensor values
          float frontSensor = Sample::getValue(POS_FRONT);
          float rightSensor = Sample::getValue(POS_RIGHT);
          float leftSensor = Sample::getValue(POS_LEFT);

          if (rightSensor > IR_THRESHOLD)
          {
            // Rotate towards the sample
            Driving::addCallback(-1, onRotatingFinish);
            Driving::rotate(7 * DEGREE_PER_PULSE);
          }

          if (leftSensor > IR_THRESHOLD)
          {
            // Rotate towards the sample
            Driving::addCallback(-1, onRotatingFinish);
            Driving::rotate(-7 * DEGREE_PER_PULSE);
          }

          if (frontSensor > IR_THRESHOLD)
          {
            Wireless::setVariable(VAR_STATUS, VERIFYING_SAMPLE);
          }
        }
        else if (subState == SUB_ROTATING_COMMAND)
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
        else if (subState == SUB_ROTATING_FINISH)
        { 
          subState = SUB_DRIVING_COMMAND;
        }
    }        
      break;

    case VERIFYING_SAMPLE:
    {
      // Check with UDS if it is the base
      if (UDS::distanceAtDegree(UDS_ANGLE_BASE) < UDS_COLLISION_DISTANCE)
      {
        // Go back to searching sample and rotate away from the obstacle
        Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
        subState = SUB_ROTATING_COMMAND;
      }
      else
      {
        Wireless::setVariable(VAR_STATUS, PICKING_UP_SAMPLE);
      }
    }
      break;

    case PICKING_UP_SAMPLE:
    {
      float frontSensor = Sample::getValue(POS_FRONT);
      // If the robot is close enough start trying picking up the sample
      if (frontSensor > IR_PICKUP_THRESHOLD)
      {
        // Stop driving and open the gripper
        Driving::drive(0);
        Gripper::open();

        // Drive a little bit forward to get the sample between the gripper
        Driving::addCallback(PICKUP_PULSES, onDrivingFinish);
        Driving::drive(1);
      }
      // Screwed up picking up the sample
      else if (frontSensor < IR_THRESHOLD)
      {
        Gripper::idle();
        Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
        subState = SUB_ROTATING_COMMAND;
      }

      if (subState == SUB_DRIVING_FINISH)
      {
        // Close the gripper to pick up the sample
        Gripper::close();

        // Check if the robot actualy picked up the sample
        if (frontSensor > IR_THRESHOLD)
        {
          // Did not pick it up; start driving forward
          Driving::drive(0);
          subState == SUB_DRIVING;
        }
        else
        {
          // Picked up the sample; go find the base!
          Wireless::setVariable(VAR_STATUS, SEARCHING_BASE);
          subState = SUB_DRIVING_COMMAND;
        }
      }
    }
      break;

    case SEARCHING_BASE:
    {
      //
    }
      break;

    case DRIVING_UP_BASE:
    {
      //
    }
      break;

    case DROPPING_SAMPLE:
    {
      //
    }
      break;

    case DRIVING_DOWN_BASE:
    {
      //
    }
      break;

    case DONE:
    {
      if (subState == SUB_CLAP)
        // Start clapping to celebrate this extraordinary presentation!
        Gripper::clapYourHands();
      else
      {
        // Stop driving     
        Driving::drive(0);
        subState = SUB_CLAP;
      }
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
