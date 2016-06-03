#include "Definitions.h"
#include "WirelessSerial.h"

// Variables
int Wireless::NumberContainer[VARIABLE_COUNT];
int Wireless::RemoteContainer[VARIABLE_COUNT];

// Buffer
byte Wireless::writePointer;
byte Wireless::packetId;
char Wireless::readBuffer[16];

// Initialization
void Wireless::open()
{
  // Fill up variables with default values
  NumberContainer[VAR_STATUS] = START_ON_BASE; // Status of the Robot
  NumberContainer[VAR_SAMPLES] = 8; // Number of samples left to search for

  // Check our DebugSerial
#ifndef __DEBUG_SERIAL
  // Open Serial communications
  Serial.begin(9600);
#endif // __DEBUG_SERIAL
}

// Serial reading function
void Wireless::read()
{
  // Check if we have serial data
  if (Serial.available() > 0)
  {
    // Get the byte
    byte code = Serial.read();
    
    // Check for newline
    if (code == PACKET_ENDING)
    {
      // Get the packet
      char packet = readBuffer[0];
      
      // Match commands
      if (packet == PACKET_REQUEST)
      {
        Driving::rotate(value);
      }
      
      // Clear the input
      memset(readBuffer, 0, strlen(readBuffer));
      
      // Reset pointer
      writePointer = 0;
    }
    else
    {
      readBuffer[writePointer++] = code;
    }
  }
}

// Number setter
void Wireless::setVariable(byte id, int value, bool remote)
{
  if (remote)
    RemoteContainer[id] = value;
  else
    NumberContainer[id] = value;
}

// Number getter
int Wireless::getVariable(byte id, bool remote)
{
  if (remote)
    return RemoteContainer[id];
  else
    return NumberContainer[id];
}

// Temporary message receiver
void Wireless::onReceiveMsg(byte packet, byte id, int value)
{
  switch (packet)
  {
    case PACKET_SYNC:
      // 
      break;

    case PACKET_REQUEST:
      // 
      break;

    case PACKET_INCREASE:
      setVariable(id, getVariable(id) + value);
      break;

    case PACKET_DECREASE:
      setVariable(id, getVariable(id) - value);
      break;
  }
}
