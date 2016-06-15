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
volatile RobotSubStatus subState = SUB_START;
volatile RobotSubStatus prevSubState = subState;
int UDSDegree = UDS_SWEEP_MIN;
bool UDSSweepDirection = 0;

// Setup function
void setup()
{
  // Enable interrupts for the wheel encoders and the line sensors
  enableInterrupts();

  // Take a seed from a not used analog pin to make the random values more random
  randomSeed(analogRead(A0));

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

// Get the robot substate into the debug
RobotSubStatus DebugSerial::getSubStatus()
{
  return subState;
}

// Set the robot substate from the line
void Line::setSubState(RobotSubStatus state)
{
  prevSubState = subState;
  subState = state;
}

// Set the robot substate
void setSubState(RobotSubStatus state)
{
  prevSubState = subState;
  subState = state;
}

// Rotate callback
void onRotatingFinish()
{
  setSubState(SUB_ROTATING_FINISH);
#ifdef __DEBUG_SERIAL
  Serial.println("Finished rotating!");
#endif // __DEBUG_SERIAL
}

// Drive callback
void onDrivingFinish()
{
  setSubState(SUB_DRIVING_FINISH);
#ifdef __DEBUG_SERIAL
  Serial.println("Finished driving!");
#endif // __DEBUG_SERIAL
}

// Right line collision callback
void rightLineCallback()
{
  // Slightly random number of rotation pulses
  int rotationPulses = -1 * COLLISION_ROTATION_PULSES + random(COLLISION_RANDOM_ROTATION_PULSES);

  // Start rotating and set callback
  Driving::rotate(rotationPulses * DEGREE_PER_PULSE, -1, onRotatingFinish, 208);
  setSubState(SUB_ROTATING);
}

// Right line collision callback
void leftLineCallback()
{
  // Slightly random number of rotation pulses
  int rotationPulses = COLLISION_ROTATION_PULSES - random(COLLISION_RANDOM_ROTATION_PULSES);

  // Start rotating and set callback
  Driving::rotate(rotationPulses * DEGREE_PER_PULSE, -1, onRotatingFinish, 209);
  setSubState(SUB_ROTATING);
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

        setSubState(SUB_DRIVING_COMMAND);
      }
      else if (subState == SUB_DRIVING_COMMAND)
      {
        // This robot is the last robot
        if (UDS::distanceAtDegree(UDS_ANGLE_BASE) < UDS_ROBOT_FRONT_DISTANCE)
        {
          // Wait x seconds to make sure the first robot is on its way
          delay(LAST_ROBOT_WAIT_TIME);

          // Drive the needed pulses forward to get off the base
          Driving::drive(1, LAST_OFF_BASE_PULSES, onDrivingFinish, 100);
        }
        // This robot is the first robot
        else
        {
          // Drive the needed pulses forward to get off the base
          Driving::drive(1, FIRST_OFF_BASE_PULSES, onDrivingFinish, 101);
        }

        setSubState(SUB_DRIVING);
      }
      else if (subState == SUB_DRIVING_FINISH)
      {
        // Done driving off the base, go to next state
        setSubState(SUB_ROTATING_COMMAND);
      }
      else if (subState == SUB_ROTATING_COMMAND)
      {
        // Rotate to drive to a random direction
        Driving::rotate(random(-5, 6) * DEGREE_PER_PULSE, -1, onRotatingFinish, 200);

        setSubState(SUB_ROTATING);
      }
      else if (subState == SUB_ROTATING_FINISH)
      {
        // Go to the next state and search for a sample
        Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
        setSubState(SUB_DRIVING_COMMAND);
      }
      else if (subState == SUB_RIGHT_LINE)
      {
        setSubState(prevSubState);
      }
      else if (subState == SUB_LEFT_LINE)
      {
        setSubState(prevSubState);
      }
    }
      break;

    case SEARCHING_SAMPLE:
    {
        if (subState == SUB_DRIVING_COMMAND)
        {
          Driving::drive(1, 0, NULL, 102);
          setSubState(SUB_DRIVING);
        }
        else if (subState == SUB_DRIVING)
        {
          // Almost colliding with a mountain so rotate
          if (UDS::pollForDistance() < UDS_COLLISION_DISTANCE)
          {
#ifdef __DEBUG_SERIAL
            Serial.println("UDS triggered!");
#endif // __DEBUG_SERIAL
            
            // To-Do: Depending on how close we are, maybe reverse            
            setSubState(SUB_ROTATING_COMMAND);
          }

          /*
          // Read the IR sensor values
          float frontSensor = Sample::getValue(POS_FRONT);
          float rightSensor = Sample::getValue(POS_RIGHT);
          float leftSensor = Sample::getValue(POS_LEFT);

          if (rightSensor > IR_THRESHOLD)
          {
            // Rotate towards the sample
            Driving::rotate(7 * DEGREE_PER_PULSE, -1, onRotatingFinish);
          }

          if (leftSensor > IR_THRESHOLD)
          {
            // Rotate towards the sample
            Driving::rotate(-7 * DEGREE_PER_PULSE, -1, onRotatingFinish);
          }

          if (frontSensor > IR_THRESHOLD)
          {
            Wireless::setVariable(VAR_STATUS, VERIFYING_SAMPLE);
          }
          */
        }
        else if (subState == SUB_ROTATING_COMMAND)
        {
          // Choose a random rotation direction
          long randomNumber = random(2);
          if (randomNumber == 0)
            Driving::rotate(6 * DEGREE_PER_PULSE, -1, onRotatingFinish, 201);
          else
            Driving::rotate(-6 * DEGREE_PER_PULSE, -1, onRotatingFinish, 202);

          setSubState(SUB_ROTATING);
        }
        else if (subState == SUB_ROTATING_FINISH)
        { 
          setSubState(SUB_DRIVING_COMMAND);
        }
        else if (subState == SUB_RIGHT_LINE)
        {
          Serial.println("Sub right jognuh");
          setSubState(SUB_REVERSE_COMMAND);
        }
        else if (subState == SUB_LEFT_LINE)
        {
          Serial.println("Sub left jognuh");
          setSubState(SUB_REVERSE_COMMAND);
        }
        else if (subState == SUB_REVERSE_COMMAND)
        {
          if (prevSubState == SUB_RIGHT_LINE)
          {
            Driving::drive(-1, COLLISION_REVERSE_PULSES, rightLineCallback, 111);
          }
          else if (prevSubState == SUB_LEFT_LINE)
          {
            Driving::drive(-1, COLLISION_REVERSE_PULSES, leftLineCallback, 112);
          }

          setSubState(SUB_REVERSE);
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
        setSubState(SUB_ROTATING_COMMAND);
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
        Driving::drive(0, 0, NULL, 103);
        Gripper::open();

        // Drive a little bit forward to get the sample between the gripper
        Driving::drive(1, PICKUP_PULSES, onDrivingFinish, 104);
      }
      // Screwed up picking up the sample
      else if (frontSensor < IR_THRESHOLD)
      {
        Gripper::idle();
        Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
        setSubState(SUB_ROTATING_COMMAND);
      }

      if (subState == SUB_DRIVING_FINISH)
      {
        // Close the gripper to pick up the sample
        Gripper::close();

        // Check if the robot actualy picked up the sample
        if (frontSensor > IR_THRESHOLD)
        {
          // Did not pick it up; start driving forward
          Driving::drive(0, 0, NULL, 105);
          setSubState(SUB_DRIVING);
        }
        else
        {
          // Picked up the sample; go find the base!
          Wireless::setVariable(VAR_STATUS, SEARCHING_BASE);
          setSubState(SUB_DRIVING_COMMAND);
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
        Driving::drive(0, 0, NULL, 106);
        setSubState(SUB_CLAP);
      }
    }
      break;
  }

//  delay(100);
//  Serial.print("State: ");
//  Serial.print(robotStatus);
//  Serial.print("\tSub state: ");
//  Serial.println(subState);
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
