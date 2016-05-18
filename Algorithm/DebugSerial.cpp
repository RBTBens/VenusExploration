#include "Definitions.h"
#ifdef __DEBUG
#include "DebugSerial.h"

// Constructor
DebugSerial::DebugSerial(Driving e)
{  
  // Set the engine variable from Algorithm
  engine = e;
}

// Initialization function
void DebugSerial::open()
{
  // Open Serial communications
  Serial.begin(9600);
  Serial.println("Robot is alive!");

  // Print all functions
  Serial.println("Debugging options (End all lines with ;)");
  Serial.println("- a: rotate func [degrees]");
  Serial.println("- b: drive func [pulses]");
  Serial.println("- c: set power func [value]");
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
  // Check if we're handling any input
  if (nDebugItem == 0)
  {
    // See which function we're sending data to
    switch (code)
    {
      case 'a': nDebugItem = 1; break;
      case 'b': nDebugItem = 2; break;
      case 'c': nDebugItem = 3; break;
      default: break;
    }

    // Set write pointer
    nWritePointer = 0;
  }
  else
  {
    // Terminator character
    if (code == ';')
    {
      // Translate the ASCII digits to a float
      float value = translate(szDebugStr, nWritePointer);
      
      // Execute code accordingly
      if (nDebugItem == 1)
      {
        engine.rotate(value);
      }
      else if (nDebugItem == 2)
      {
        engine.drive(1, (int)value);
      }
      else if (nDebugItem == 3)
      {
        //engine.setPower((int)value);
      }

      // Reset the ID
      nDebugItem = 0;
    }
    else
      szDebugStr[nWritePointer++] = code;
  }
}

// Translate input data
float DebugSerial::translate(char* data, byte point)
{
  float value = 0;
  byte start = 0;
  bool negate = false;

  if (szDebugStr[start] == ' ')
    start++;

  if (szDebugStr[start] == '-')
  {
    start = 1;
    negate = true;
  }
  
  for (int i = start; i < point; i++)
    value += (szDebugStr[i] - 48) * pow(10, nWritePointer - i - 1);

  if (negate)
    return -value;
  else
    return value;
}

#endif // __DEBUG
