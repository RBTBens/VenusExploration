#ifndef DebugSerial_h
#define DebugSerial_h
#include "Driving.h"
#include "UDS.h"
#include "Sample.h"

// The class interface
class DebugSerial
{
private:
  static byte writePointer;
  static byte debugItem;
  static char debugStr[16];
  
public:
  static void open();
  static void read();
  static void handle(byte code);
  static float getToken(char* data, byte offset, byte ending);
};

#endif // DebugSerial_h
