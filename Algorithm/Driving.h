#ifndef Driving_h
#define Driving_h
#include <Servo.h>

// The class interface
class Driving
{
private:
  static Servo leftWheel;
  static Servo rightWheel;

  static volatile int leftPulses;
  static volatile int rightPulses;
  static volatile int rotatePulses;

  static double relativeXPosition;
  static double relativeYPosition;
  static double relativeOrientation;

  static void (*rotateCallback)();

public:
  static void initialize();
  static void trigger(byte pin);
  static void rotate(float degree, void (*callback)());
  static void drive(int dir);
  static void calculateNewPosition(int degreeTurned, int pulsesDriven);
  static double* calculateBaseDirection();
  static void resetPosition();
};

#endif // Driving_h
