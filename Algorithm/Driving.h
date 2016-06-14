#ifndef Driving_h
#define Driving_h

// The class interface
class Driving
{
private:
  static Servo leftWheel;
  static Servo rightWheel;

  static volatile int leftPulses;
  static volatile int rightPulses;
  static volatile int rotatePulses;

  static int callbackId;
  static int callbackPulses;
  static void (*callbackFunc)();

public:
  static double relativeXPosition;
  static double relativeYPosition;
  static double relativeOrientation;

  static void initialize();
  static void trigger(byte pin);
  static void rotate(float degree, int pulses = 0, void (*callback)() = NULL, int id = 0);
  static void drive(int dir, int pulses = 0, void (*callback)() = NULL, int id = 0);

  static void addCallback(int pulses, void (*callback)(), int id = 0);
  static void runCallback();

  static void mapAndReset();
  static void calculateNewPosition(int degreeTurned, int pulsesDriven);
  static double* calculateBaseDirection();
  static void resetPosition();
};

#endif // Driving_h
