#ifndef Gripper_h
#define Gripper_h
#include "Arduino.h"
#include <Servo.h>

// Compile-time numbers
#define ID_GRIPPERSERVO 10

// The class interface
class Gripper
{
public:
  Gripper();
  ~Gripper();

  void initialize();
};

#endif // Gripper_h