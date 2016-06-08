#ifndef Line_h
#define Line_h
#include "Driving.h"

// The class interface
class Line
{
public:
  static void trigger(byte pin);

  static void onLeftReverse();
  static void onRightReverse();
  static void onDrivingFinish();
  static void onRotateFinish();
  static void onRotateFinishBackward();
};

#endif // Line_h

