#include "Definitions.h"
#include "Wireless.h"

// Variables
int Wireless::NumberContainer[VARIABLE_COUNT];

// Initialization
void Wireless::initialize()
{
  // Probably use Serial here
  // Make sure the ZigBee is able to communicate

  // Fill up variables with default values
  NumberContainer[VAR_STATUS] = START_ON_BASE; // Status of the Robot
  NumberContainer[VAR_SAMPLES] = 8; // Number of samples left to search for
}

// Number setter
void Wireless::setVariable(byte id, int value)
{
  NumberContainer[id] = value;
}

// Number getter
int Wireless::getVariable(byte id)
{
  return NumberContainer[id];
}

// Temporary message receiver
void onReceiveMsg(byte packet, byte id, int value)
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
