#include "Definitions.h"
#include "DebugSerial.h"

// Variables
byte debugItem = 0;
byte writePointer = 0;
char debugStr[8];

// Constructor
DebugSerial::DebugSerial(Driving e)
{
  engine = e;
}

// Initialization function
void DebugSerial::open()
{
  Serial.println("Debugging options (End all lines with ;)");
  Serial.println("- a: rotate func [degrees]");
  Serial.println("- b: drive func [pulses]");
}

// Serial input handler
void DebugSerial::handle(byte code)
{
  // Check if we're handling any input
  if (debugItem == 0)
  {
    // See which function we're sending data to
    switch (code)
    {
      case 'a': debugItem = 1; break;
      case 'b': debugItem = 2; break;
      default: break;
    }

    // Set write pointer
    writePointer = 0;
  }
  else
  {
    // Terminator character
    if (code == ';')
    {
      // Translate the ASCII digits to a float
      float value = translate(debugStr, writePointer);
      
      // Execute code accordingly
      if (debugItem == 1)
      {
        engine.rotate(value);
      }
      else if (debugItem == 2)
      {
        engine.drive(1, (int)value);
      }

      // Reset the ID
      debugItem = 0;
    }
    else
      debugStr[writePointer++] = code;
  }
}

float DebugSerial::translate(char* data, byte point)
{
  float value = 0;
  byte start = 0;
  bool negate = false;

  if (debugStr[start] == '-')
  {
    start = 1;
    negate = true;
  }
  
  for (int i = start; i < point; i++)
    value += (debugStr[i] - 48) * pow(10, writePointer - i - 1);

  if (negate)
    return -value;
  else
    return value;
}
