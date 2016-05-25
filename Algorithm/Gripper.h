#ifndef Gripper_h
#define Gripper_h
#include <Servo.h>

// The class interface
class Gripper
{
private:
  static Servo gripperServo;

public:
  static void initialize();
};

#endif // Gripper_h

