#ifndef Line_h
#define Line_h
#include "Driving.h"

// The class interface
class Line
{
private:
  static subStateSetter setSubState;

public:
  static void link(subStateSetter set);
  static void trigger(byte pin);
};

#endif // Line_h

