#include "Definitions.h"
#ifdef __DEBUG
#include "DebugSerial.h"

// Constructor
DebugSerial::DebugSerial()
{
}

// Initialization function
void DebugSerial::open()
{
  // Open Serial communications
  Serial.begin(9600);
  Serial.println("Robot is alive!");

  // Print all functions
  Serial.println("Debugging options (End all lines with ;)");
  Serial.println("- rot [degrees]");
  Serial.println("- drive [direction]");
  Serial.println("- uds [width]");
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
    int nSpace = 0;
    for (int i = 0; i < strlen(szDebugStr); i++)
    {
      if (szDebugStr[i] == ' ')
        nSpace = i;
    }
    
    // Get the command
    char buff[nSpace + 1];
    memcpy(buff, szDebugStr, nSpace);
    buff[nSpace] = '\0';

    // Get the parameters
    int value = getToken(szDebugStr, nSpace + 1, nWritePointer);
    
    // Match commands
    if (strcmp(buff, "rot") == 0)
    {
      Driving* engine = Driving::getInstance();
      engine->rotate(value);
    }
    else if (strcmp(buff, "drive") == 0)
    {
      Driving* engine = Driving::getInstance();
      engine->drive(value);
    }
    else if (strcmp(buff, "uds") == 0)
    {
      UDS* uds = UDS::getInstance();
      long* list = uds->distanceOfRangeOfDegrees(0, 90);

      for (int i = 0; i < 90; i++)
      {
        Serial.println(list[i]);
      }
    }
    else
    {
      Serial.print("Unrecognized command: ");
      Serial.println(buff);
    }

    // Clear the input
    memset(szDebugStr, 0, strlen(szDebugStr));

    // Reset pointer
    nWritePointer = 0;
  }
  else
  {
    szDebugStr[nWritePointer++] = code;
  }
}

// Translate input data
int DebugSerial::getToken(char* data, byte offset, byte ending)
{
  // Set defaults
  int value = 0;
  byte start = offset;
  bool negate = false;

  // Check if it's a negative number
  if (szDebugStr[start] == '-')
  {
    start++;
    negate = true;
  }

  // Get the powers of each number
  for (int i = start; i < ending; i++)
    value += (szDebugStr[i] - 48) * (int)pow(10, ending - i - 1);

  // Return correct value
  return negate ? -value : value;
}

#endif // __DEBUG
