#ifndef Driving_h
#define Driving_h
#include <Servo.h>

// Compile-time numbers
#define ID_LEFTENCODER 7
#define ID_RIGHTENCODER 8
#define ID_LEFTSERVO 12
#define ID_RIGHTSERVO 13

#define LEFT_FORWARD 108
#define LEFT_REVERSE 76
#define RIGHT_FORWARD 76
#define RIGHT_REVERSE 108

#define SERVO_NEUTRAL 92
#define SERVO_DIFFERENCE 16
#define DRIVE_BRAKE_PULSES 5.0
#define DEGREE_PER_PULSE 13.11

// The class interface
class Driving
{
public:
  Driving();

  void initialize();
  void loop();
  
  void rotate(float degree);
  void drive(int dir);
  void drive(int dir, int pulses);
  void setPower(int value);

private:
  byte adjustWheelRate(int pulses, byte input);
  
  void leftEncoderPulse();
  void rightEncoderPulse();
};

#endif // Driving_h
