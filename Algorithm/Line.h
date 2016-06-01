#ifndef Line_h
#define Line_h
#include "Driving.h"

// The class interface
class Line
{
public:
  static bool isOverLine();
  static void trigger(byte pin);
};

#endif // Line_h

