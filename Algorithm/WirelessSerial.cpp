#include "Definitions.h"
#include "WirelessSerial.h"

// Variables
int Wireless::NumberContainer[VAR_COUNT];
int Wireless::RemoteContainer[VAR_COUNT];

// Buffer
byte Wireless::writePointer;
byte Wireless::packetId;
byte Wireless::readBuffer[BUFFER_LENGTH];

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
      onReceiveMsg(readBuffer);
      
      // Clear the input
      memset(readBuffer, 0, BUFFER_LENGTH);
      
      // Reset pointer
      writePointer = 0;
    }
    else
    {
      // Append to the buffer
      readBuffer[writePointer++] = code;
    }
  }
}

// Number setter
void Wireless::setVariable(byte id, int value, bool submit)
{
  NumberContainer[id] = value;

  // See if we have to send it over
  if (submit)
  {
    // Convert value to byte array
    byte* arr = (byte*)&value;
    
    // Construct the packet
    byte sendLen = 7;
    byte sendBuf[sendLen];
    sendBuf[0] = PACKET_SYNC;
    sendBuf[1] = id;

    // Add the integer value
    for (int i = 0; i < 4; i++)
      sendBuf[2 + i] = arr[i];

    // End the packet
    sendBuf[6] = PACKET_ENDING;
    
    // Write it
    Serial.write(sendBuf, sendLen);
  }
}

// Number getter
int Wireless::getVariable(byte id, bool remote)
{
  if (remote)
    return RemoteContainer[id];
  else
    return NumberContainer[id];
}

// Message receiver
void Wireless::onReceiveMsg(byte buf[])
{
  // Grab the first byte
  char packet = buf[0];

  // See which packet it is
  switch (packet)
  {
    case PACKET_SYNC:
      // Update it in the remote array
      RemoteContainer[buf[1]] = *((int*)&buf[2]);
      break;
  }
}
