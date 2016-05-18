#ifndef Driving_h
#define Driving_h
#include <Servo.h>

// Compile-time numbers
#define ID_LEFTENCODER 7
#define ID_RIGHTENCODER 8
#define ID_LEFTSERVO 12
#define ID_RIGHTSERVO 13

#define LEFT_FORWARD 106
#define LEFT_REVERSE 74
#define RIGHT_FORWARD 74
#define RIGHT_REVERSE 106

#define SERVO_NEUTRAL 92
#define SERVO_DIFFERENCE 16
#define DRIVE_BRAKE_PULSES 5.0
#define MAX_PULSE_TIME 250
#define DEGREE_PER_PULSE 13.11

// The class interface
class Driving
{
public:
  Driving();

  void initialize();
  void trigger(byte pin, bool state);
  void rotate(float degree);
  void drive(int dir);
  void drive(int dir, int pulses);
};

#endif // Driving_h
