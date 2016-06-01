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

  static int callbackPulses;
  static void (*callbackFunc)();

public:
  static void initialize();
  static void trigger(byte pin);
  static void rotate(float degree);
  static void drive(int dir);

  static void addCallback(int pulses, void (*callback)());
  static void runCallback();
  
  static void calculateNewPosition(int degreeTurned, int pulsesDriven);
  static double* calculateBaseDirection();
  static void resetPosition();
};

#endif // Driving_h
