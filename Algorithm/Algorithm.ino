// Main includes
#include "Definitions.h"
#include "Driving.h"
#include "Gripper.h"
#include "UDS.h"
#include "Line.h"
#include "Beacon.h"
#include "Sample.h"

// Debug includes
#ifdef __DEBUG_SERIAL
#include "DebugSerial.h"
#endif // __DEBUG_SERIAL

// Variables
volatile RobotSubStatus subState = SUB_START;
volatile RobotSubStatus prevSubState = subState;

// Trackers
PulseTracker trackBase;
PulseTracker trackSearchRotate;
PulseTracker trackSearchReverse;
PulseTracker trackPickup;
PulseTracker trackReturn;

// Robot sub state interaction
RobotSubStatus getSubState() { return subState; }
void setSubState(RobotSubStatus state) { prevSubState = subState; subState = state; }

// Setup function
void setup()
{
  // Enable interrupts for the wheel encoders and the line sensors
  enableInterrupts();

  // Take a seed from a not used analog pin to make the random values more random
  randomSeed(analogRead(A0));

  // Open talking to Serial
#ifdef __DEBUG_SERIAL
  DebugSerial::open(getSubState);
#endif // __DEBUG_SERIAL

  // Open the wireless
  Wireless::open();

  // Start up the engine
  Driving::initialize();

  // Activate the UDS
  UDS::initialize();

  // Turn on the gripper
  Gripper::initialize();

  // Link the line subStateSetter
  Line::link(setSubState);
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
          trackBase = Driving::startMeasurement(LAST_OFF_BASE_PULSES);
        }
        // This robot is the first robot
        else
        {
          // Drive the needed pulses forward to get off the base
          trackBase = Driving::startMeasurement(FIRST_OFF_BASE_PULSES);
        }

        // And go!
        Driving::drive(1);
        setSubState(SUB_DRIVING);
      }
      else if (subState == SUB_DRIVING)
      {
        // Done driving off the base, go to next state
        if (trackBase.hasCompleted())
          setSubState(SUB_ROTATING_COMMAND);
      }
      else if (subState == SUB_ROTATING_COMMAND)
      {
        // Reset this position as home
        Driving::resetPositionForward();
        
        // Rotate to drive to a random direction
        int pulses = random(1, 6);
        trackBase = Driving::startMeasurement(pulses);
        Driving::rotate();
        
        setSubState(SUB_ROTATING);
      }
      else if (subState == SUB_ROTATING)
      {
        if (trackBase.hasCompleted())
        {
          // Stop rotating
          Driving::drive(0);
          
          // Go to the next state and search for a sample
          Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
          setSubState(SUB_DRIVING_COMMAND);
        }
      }
      else if (subState == SUB_LEFT_LINE || subState == SUB_RIGHT_LINE)
      {
        // In this state, we will ignore the line sensors
        setSubState(prevSubState);
      }
    }
      break;

    case SEARCHING_SAMPLE:
    {
        if (subState == SUB_DRIVING_COMMAND)
        {
          // Start driving forward indefinitely
          Driving::drive(1);
          setSubState(SUB_DRIVING);
        }
        else if (subState == SUB_DRIVING)
        {
          // Make the UDS run / sweep and read its value
          long UDSDistance = UDS::pollForDistance();
          
          // Almost colliding with a mountain so rotate
          if (UDSDistance < UDS_COLLISION_DISTANCE)
          {
#ifdef __DEBUG_SERIAL
            Serial.println("UDS triggered!");
#endif // __DEBUG_SERIAL
            
            // Too close to mountain so first reverse
            if (UDSDistance < UDS_COLLISION_REVERSE_DISTANCE)
              setSubState(SUB_REVERSE_COMMAND);
            
            // Not so close to moutain so rotate
            else
              setSubState(SUB_ROTATING_COMMAND);
          }

          // Read the IR sensor values
          float frontSensor = Sample::getValue(POS_FRONT);
          float rightSensor = Sample::getValue(POS_RIGHT);
          float leftSensor = Sample::getValue(POS_LEFT);

          // Check if it's past the front threshold
          if (frontSensor > IR_FRONT_THRESHOLD)
          {
            Wireless::setVariable(VAR_STATUS, VERIFYING_SAMPLE);
          }
#ifdef __IR_SIDE_CHECK
          else if (rightSensor > IR_SIDE_THRESHOLD)
          {
            // Rotate towards the sample
            trackSearchRotate = Driving::startMeasurement(SIDE_IR_SAMPLE_ROTATE);
            Driving::rotate(1);

            setSubState(SUB_ROTATING);
          }
          else if (leftSensor > IR_SIDE_THRESHOLD)
          {
            // Rotate towards the sample
            trackSearchRotate = Driving::startMeasurement(SIDE_IR_SAMPLE_ROTATE);
            Driving::rotate(-1);

            setSubState(SUB_ROTATING);
          }
#endif // __IR_SIDE_CHECK
        }
        else if (subState == SUB_ROTATING_COMMAND)
        {
          // Start a tracker for the required amount of pulses
          trackSearchRotate = Driving::startMeasurement(UDS_ROTATION_PULSES);
          Driving::rotate();
          
          setSubState(SUB_ROTATING);
        }
        else if (subState == SUB_ROTATING)
        {
          // After we have completed, give a driving command again
          if (trackSearchRotate.hasCompleted())
            setSubState(SUB_DRIVING_COMMAND);
        }
        else if (subState == SUB_REVERSE_COMMAND)
        {
          // Reverse for a given amount of pulses
          trackSearchReverse = Driving::startMeasurement(UDS_REVERSE_PULSES);
          Driving::drive(-1);
          setSubState(SUB_REVERSE);
        }
        else if (subState == SUB_LEFT_LINE || subState == SUB_RIGHT_LINE)
        {
          // When we collide with a line here, we'll reverse as well
          trackSearchReverse = Driving::startMeasurement(COLLISION_REVERSE_PULSES);
          Driving::drive(-1);
          setSubState(SUB_REVERSE);
        }
        else if (subState == SUB_REVERSE)
        {
          // Check if we're done reversing
          if (trackSearchReverse.hasCompleted())
          {
            // Randomly see how far we want to rotate after reversing
            int pulses = COLLISION_ROTATION_PULSES - random(COLLISION_RANDOM_ROTATION_PULSES);
            trackSearchRotate = Driving::startMeasurement(pulses);

            // If we hit the left sensor, we'll want to rotate to the right; rotate left for the right sensor
            if (prevSubState == SUB_LEFT_LINE)
              Driving::rotate(1);
            else if (prevSubState == SUB_RIGHT_LINE)
              Driving::rotate(-1);
            else
            {
              // If we're reversing for the UDS, change to rotating
              setSubState(SUB_ROTATING_COMMAND);
              return;
            }

            // Keep track of rotating
            setSubState(SUB_ROTATING);
          }
        }
    }        
      break;

    case VERIFYING_SAMPLE:
    {
      if (subState == SUB_DRIVING)
      {
        // Read the value of the front sensor
        float frontSensor = Sample::getValue(POS_FRONT);
        
        // If the robot is close enough start trying picking up the sample
        if (frontSensor > IR_PICKUP_THRESHOLD)
        {
          if (UDS::distanceAtDegree(UDS_ANGLE_BASE) > UDS_NOSAMPLE_DISTANCE)
          {
            // Stop driving and open the gripper
            Driving::drive(0);
            Gripper::open();
  
            // Wait for the gripper to open
            delay(GRIPPER_DELAY);
  
            // Go picking up the sample
            Wireless::setVariable(VAR_STATUS, PICKING_UP_SAMPLE);
            setSubState(SUB_DRIVING_COMMAND);
          }
          else
          {
            // Messed up by trying to pick up the base; rotate back
            Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
            setSubState(SUB_ROTATING_COMMAND);
          }
        }
        // Sample is out of reach aggain; go back to search sample
        else if (frontSensor < IR_FRONT_THRESHOLD)
        {
          // If we're out of range again, go back to searching a sample
          Wireless::setVariable(VAR_STATUS, SEARCHING_SAMPLE);
        }
      }
      else if (subState == SUB_LEFT_LINE || subState == SUB_RIGHT_LINE)
      {
        // When we're verifying we should be standing still, so any line trigger would be an error; thus do nothing
        setSubState(prevSubState);
      }
    }
      break;

    case PICKING_UP_SAMPLE:
    {
      if (subState == SUB_DRIVING_COMMAND)
      {
        // Drive a little bit forward to get the sample between the gripper
        trackPickup = Driving::startMeasurement(PICKUP_PULSES);
        Driving::drive(1);
        setSubState(SUB_DRIVING);
      }
      else if (subState == SUB_DRIVING)
      {
        // Check when we're done
        if (trackPickup.hasCompleted())
        {
          // Pickup the sample
          Driving::drive(0);
          Gripper::close();

          // Wait for the gripper to close
          delay(GRIPPER_DELAY);

          // Drive a little bit backwards to check if the robot actually picked up the sample
          setSubState(SUB_REVERSE_COMMAND);
        }
      }
      else if (subState == SUB_REVERSE_COMMAND)
      {
        // Give a new reverse command
        trackPickup = Driving::startMeasurement(PICKUP_REVERSE_PULSES);
        Driving::drive(-1);
        setSubState(SUB_REVERSE);
      }
      else if (subState == SUB_REVERSE)
      {
        // When we've reversed far enough
        if (trackPickup.hasCompleted())
        {
          // Set the gripper to idle to be able measure if the sample still is in front of the robot
          Driving::drive(0);
          Gripper::idle();

          // Wait for the gripper to idle
          delay(GRIPPER_DELAY);

          // Read the front IR sensor
          float frontSensor = Sample::getValue(POS_FRONT);
                
          // Check if the robot actualy picked up the sample
          if (frontSensor > IR_FRONT_THRESHOLD)
          {
            // Did not pick it up; try aggain
            Driving::drive(1);
            Wireless::setVariable(VAR_STATUS, VERIFYING_SAMPLE);
            setSubState(SUB_DRIVING);
          }
          else
          {
            // Sample is between the grippers; pick it up aggain
            Gripper::close();

            // Wait for the gripper to close
            delay(GRIPPER_DELAY);            

            // Go find the base!
            Driving::drive(0);
            Wireless::setVariable(VAR_STATUS, SEARCHING_BASE);
            setSubState(SUB_LOCKBASE_COMMAND);
          }
        }
      }
      else if (subState == SUB_LEFT_LINE || subState == SUB_RIGHT_LINE)
      {
        // Ignore line sensors and go back to where we were
        setSubState(prevSubState);
      }
    }
      break;

    case SEARCHING_BASE:
    {
        if (subState == SUB_DRIVING_COMMAND)
        {
          // When searching base we also drive forward until we encounter something
          Driving::drive(1);
          setSubState(SUB_DRIVING);
        }
        else if (subState == SUB_DRIVING)
        {
          // Make the UDS run / sweep and read its value
          long UDSDistance = UDS::pollForDistance();
          
          // Almost colliding with a mountain so rotate
          if (UDSDistance < UDS_COLLISION_DISTANCE)
          {
#ifdef __DEBUG_SERIAL
            Serial.println("UDS to base triggered!");
#endif // __DEBUG_SERIAL
            
            // Reverse whenever we see something
            setSubState(SUB_REVERSE_COMMAND);
          }

          // Check if we're close to home
          if (Beacon::seeHome())
          {
            // Let's figure out how to drive up on this bitch!
            Driving::drive(0);
            Wireless::setVariable(VAR_STATUS, DRIVING_UP_BASE);
            setSubState(SUB_ROTATING_COMMAND);
          }
        }

        // These shouldn't be used at the moment, but keeping them in to easily add randomness (To-Do)
        else if (subState == SUB_ROTATING_COMMAND)
        {
          // Give a rotate command
          trackSearchRotate = Driving::startMeasurement(UDS_ROTATION_PULSES);
          Driving::rotate();
          
          setSubState(SUB_ROTATING);
        }
        else if (subState == SUB_ROTATING)
        {
          // When we're done, lock onto base again
          if (trackSearchRotate.hasCompleted())
            setSubState(SUB_LOCKBASE_COMMAND);
        }
        else if (subState == SUB_REVERSE_COMMAND)
        {
          // Reverse for the given amount of pulses
          trackSearchReverse = Driving::startMeasurement(UDS_REVERSE_PULSES);
          Driving::drive(-1);
          setSubState(SUB_REVERSE);
        }
        else if (subState == SUB_LEFT_LINE || subState == SUB_RIGHT_LINE)
        {
          // When we hit a line, we'll reverse first
          trackSearchReverse = Driving::startMeasurement(COLLISION_REVERSE_PULSES);
          Driving::drive(-1);
          setSubState(SUB_REVERSE);
        }
        else if (subState == SUB_REVERSE)
        {
          if (trackSearchReverse.hasCompleted())
          {
            // There should be randomness in rotating here, otherwise the robot will only drive in a straight line to the base (To-Do)
            // Something similar to this should work quite effectively since the internal vector will remain to be calculated
            
            /*
            int pulses = COLLISION_ROTATION_PULSES - random(COLLISION_RANDOM_ROTATION_PULSES);
            trackSearchRotate = Driving::startMeasurement(pulses);
            
            if (prevSubState == SUB_LEFT_LINE)
              Driving::rotate(1);
            else if (prevSubState == SUB_RIGHT_LINE)
              Driving::rotate(-1);
            else
            {
              setSubState(SUB_ROTATING_COMMAND);
              return;
            }
            */

            // After reversing lock back to the base
            setSubState(SUB_LOCKBASE_COMMAND);
          }
        }
        else if (subState == SUB_LOCKBASE_COMMAND)
        {
          // Get the direction to the base
          double* baseDirection = Driving::calculateBaseDirection();

          // Calculate the amount of pulses needed and the direction of rotation
          int pulses = round(baseDirection[0] / DEGREE_PER_PULSE);
          trackReturn = Driving::startMeasurement(abs(pulses));
          Driving::rotate(pulses);
          
          setSubState(SUB_LOCKBASE);
        }
        else if (subState == SUB_LOCKBASE)
        {
          // Once we're done rotating, go back to driving forward
          if (trackReturn.hasCompleted())
            setSubState(SUB_DRIVING_COMMAND);
        }
    }
      break;

    case DRIVING_UP_BASE:
    {
      // We'll want to start with doing a 360 to find which way the intensity is highest
      if (subState == SUB_ROTATING_COMMAND)
      {
#ifdef __DEBUG_BEACON
        Serial.println("Found the base! I'm going to spin now");
#endif // __DEBUG_BEACON

        // Sets the UDS at the given angle and delays until it's there
        UDS::distanceAtDegree(UDS_ANGLE_BASE);
        
        // Now let's spin a full circle (always clockwise)
        trackReturn = Driving::startMeasurement(BASE_FULL_CIRCLE_PULSES);
        Driving::rotate(1);
        
        setSubState(SUB_ROTATING);
      }
      else if (subState == SUB_ROTATING)
      {
        // Check while we're still rotating
        if (!trackReturn.hasCompleted())
        {
          // See how far into the rotation we are and save Beacon values
          int pulses = trackReturn.getPulses();
          Beacon::readValue(pulses);
        }
        else
        {
          // Get the maximum value we found earlier
          int maxVal = Beacon::getMaximumValue();
          
          // Check if we've passed the threshold again (To-Do: Maybe subtract like -10 for inconsistencies)
          if (Beacon::readValue() >= maxVal)
            setSubState(SUB_DRIVING_COMMAND);
          else if (trackReturn.getPulses() > BEACON_ROTATE_MAX)
            setSubState(SUB_ROTATING_COMMAND);
        }
      }
      else if (subState == SUB_DRIVING_COMMAND)
      {
        // Maybe repeat the process here after getting closer to the base (To-Do)
        trackReturn = Driving::startMeasurement(BASE_DRIVE_UP);
        
        // But for now, gas bij!
        Driving::drive(1);
        setSubState(SUB_DRIVING);
      }
      else if (subState == SUB_DRIVING)
      {
        // Once we're done driving up the base
        if (trackReturn.hasCompleted())
        {
          // Continue driving but fully trust on the UDS to measure the distance to the final wall
          if (UDS::distanceAtDegree(UDS_ANGLE_BASE) < UDS_BASE_WALL_DISTANCE)
          {
            // Stop and drop
            Driving::drive(0);
            Gripper::open();

            // Decrease count
            int amount = Wireless::getVariable(VAR_SAMPLES);
            amount--;

            // Update
            Wireless::setVariable(VAR_SAMPLES, amount);

            // See what to do next: stop or continue
            if (amount <= 0)
            {
              Wireless::setVariable(VAR_STATUS, DONE);
              setSubState(SUB_DRIVING_COMMAND);
            }
            else
              setSubState(SUB_REVERSE_COMMAND);
          }
        }
      }
      else if (subState == SUB_REVERSE_COMMAND)
      {
        // Close gripper first
        Gripper::idle();

        // Reverse off the lab
        trackReturn = Driving::startMeasurement(BASE_DRIVE_DOWN);
        Driving::drive(-1);
        setSubState(SUB_REVERSE);
      }
      else if (subState == SUB_REVERSE)
      {
        // When we're done we're still facing the lab
        if (trackReturn.hasCompleted())
        {
          // Maybe we should rotate 180 degrees before continuing here (To-Do)

          // And start over again
          Wireless::setVariable(VAR_STATUS, START_ON_BASE);
          setSubState(SUB_ROTATING_COMMAND);
        }
      }
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
      if (subState == SUB_DRIVING_COMMAND)
      {
        // Stop driving     
        Driving::drive(0);
        setSubState(SUB_CLAP);
      }
      else if (subState == SUB_CLAP)
        // Start clapping to celebrate this extraordinary presentation!
        Gripper::clapYourHands();
      else if (subState == SUB_LEFT_LINE || subState == SUB_RIGHT_LINE)
      {
        setSubState(prevSubState);
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
