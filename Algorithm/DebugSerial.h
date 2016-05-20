#ifndef DebugSerial_h
#define DebugSerial_h
#include "Driving.h"
#include "UDS.h"

// The class interface
class DebugSerial
{
private:
  byte nDebugItem;
  byte nWritePointer;
  char szDebugStr[16];
  
public:
  DebugSerial();

  void open();
  void read();
  void handle(byte code);

  int getToken(char* data, byte offset, byte ending);
};

#endif // DebugSerial_h
