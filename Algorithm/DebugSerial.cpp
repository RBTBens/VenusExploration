#include "Definitions.h"
#ifdef __DEBUG_SERIAL
#include "DebugSerial.h"

// Variable initialization
byte DebugSerial::writePointer;
byte DebugSerial::debugItem;
char DebugSerial::debugStr[16];

// Initialization function
void DebugSerial::open()
{
  // Open Serial communications
  Serial.begin(9600);

  // Print all functions
  Serial.println("Debugging commands");
  Serial.println("- rot [degrees]");
  Serial.println("- drive [direction]");
  Serial.println("- dist [degree]");
  Serial.println("- sample [test]");
  Serial.println("- gripper [0/1/2]");
  Serial.println("- state [id]");
  Serial.println("- retVec");
  Serial.println("- debugStatus");
}

// Serial reading function
void DebugSerial::read()
{
  // Check if we have serial data
  if (Serial.available() > 0)
  {
    handle(Serial.read());
  }
}

// Serial input handler
void DebugSerial::handle(byte code)
{
  // Check for newline
  if (code == '\n')
  {
    // Find the first space
    int space = 0;
    for (int i = 0; i < strlen(debugStr); i++)
    {
      if (debugStr[i] == ' ')
        space = i;
    }
    
    // Get the command
    char buff[space + 1];
    memcpy(buff, debugStr, space);
    buff[space] = '\0';

    // Get the parameters
    float value = getToken(debugStr, space + 1, writePointer);
    
    // Match commands
    if (strcmp(buff, "rot") == 0)
    {
      //Driving::rotate(value, 0, NULL, 203);
    }
    else if (strcmp(buff, "drive") == 0)
    {
      //Driving::drive(value, 0, NULL, 107);
    }
    else if (strcmp(buff, "dist") == 0)
    {
      long dist = UDS::distanceAtDegree(value);

      Serial.print("Measured distance at ");
      Serial.print(value);
      Serial.print(" deg -> ");
      Serial.println(dist);
    }
    else if (strcmp(buff, "sample") == 0)
    {
      Sample::setupDetectors();

      int enumId = (int)value;
      Serial.print("Sensor value: ");
      Serial.println(Sample::getValue((SensorPos)enumId));
    }
    else if (strcmp(buff, "gripper") == 0)
    {
      int id = (int)value;
      if (id == 0) Gripper::open();
      else if (id == 1) Gripper::close();
      else if (id == 2) Gripper::idle();
    }
    else if (strcmp(buff, "state") == 0)
    {
      Wireless::setVariable(VAR_STATUS, (int)value);
    }
    else if (strcmp(debugStr, "state") == 0)
    {
      Serial.print("State: ");
      Serial.println((RobotStatus)Wireless::getVariable(VAR_STATUS));
      Serial.print("Substate: ");
      Serial.println((RobotSubStatus)getSubStatus());
    }
    else if (strcmp(debugStr, "retVec") == 0)
    {
      double* baseDirection = Driving::calculateBaseDirection();
      
      Serial.print("Rotation needed to face the base: ");
      Serial.println(baseDirection[0]);
      Serial.print("Pulses needed to get to the base: ");
      Serial.println(baseDirection[1]);
    }
    else if (strcmp(debugStr, "debugStatus") == 0)
    {
      Wireless::setVariable(VAR_STATUS, DEBUG);
      Serial.println("Entered debug state!");
    }
    else
    {
      Serial.print("Unrecognized command: ");
      Serial.println(debugStr);
    }

    // Clear the input
    memset(debugStr, 0, strlen(debugStr));

    // Reset pointer
    writePointer = 0;
  }
  else
  {
    debugStr[writePointer++] = code;
  }
}

// Translate input data
float DebugSerial::getToken(char* data, byte offset, byte ending)
{
  // Set defaults
  float value = 0;
  byte start = offset;
  bool negate = false;

  // Check if it's a negative number
  if (debugStr[start] == '-')
  {
    start++;
    negate = true;
  }

  // Get the powers of each number
  for (int i = start; i < ending; i++)
    value += (debugStr[i] - 48) * pow(10, ending - i - 1);

  // Return correct value
  return negate ? -value : value;
}

#endif // __DEBUG
