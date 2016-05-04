#ifndef Driving_h
#define Driving_h
#include "Arduino.h"
#include <Servo.h>

// Compile-time numbers
#define ID_LEFTENCODER 7
#define ID_RIGHTENCODER 8
#define ID_LEFTSERVO 12
#define ID_RIGHTSERVO 13

// The class interface
class Driving
{
public:
  Driving();
  ~Driving();

  void initialize();
  void think();
};

#endif // Driving_h