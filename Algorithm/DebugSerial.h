#ifndef DebugSerial_h
#define DebugSerial_h
#include "Driving.h"

// The class interface
class DebugSerial
{
private:
  Driving engine;
  
public:
  DebugSerial(Driving e);

  void open();
  void handle(byte code);

  float translate(char* data, byte point);
};

#endif // DebugSerial_h
