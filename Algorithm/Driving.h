#ifndef Driving_h
#define Driving_h

// Compile-time numbers
#define ID_LEFTENCODER 7
#define ID_RIGHTENCODER 8
#define ID_LEFTSERVO 12
#define ID_RIGHTSERVO 13

#define LEFT_FORWARD 180
#define LEFT_REVERSE 0
#define SERVO_NEUTRAL 92
#define RIGHT_FORWARD 0
#define RIGHT_REVERSE 180

// The class interface
class Driving
{
public:
  Driving();

  void initialize();
  
  void rotate(short degree);
  void drive(byte dir);
  
  void attachInterrupts(void (*left)(), void (*right)());
  void leftEncoderPulse();
  void rightEncoderPulse();
};

#endif // Driving_h
