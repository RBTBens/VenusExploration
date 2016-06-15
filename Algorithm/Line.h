#ifndef Line_h
#define Line_h
#include "Driving.h"

// The class interface
class Line
{
public:
  static void trigger(byte pin);
  static void setSubState(RobotSubStatus state);
};

#endif // Line_h

