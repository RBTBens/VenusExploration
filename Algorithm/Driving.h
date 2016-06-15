#ifndef Driving_h
#define Driving_h

// Pulse tracking
class PulseTracker
{
private:
  int startPulses;
  int neededPulses;
  
public:
  PulseTracker();
  PulseTracker(int p, int n);
  
  int getPulses();
  bool hasCompleted(int n = 0);
};

// The class interface
class Driving
{
private:
  static Servo leftWheel;
  static Servo rightWheel;

  static volatile int leftPulses;
  static volatile int rightPulses;
  static volatile int forwardPulses;
  static volatile bool hadLeft;
  static volatile bool hadRight;
  
  static double relativeXPosition;
  static double relativeYPosition;
  static double relativeOrientation;
  
public:
  static void initialize();
  static void trigger(byte pin);
  static void rotate(float degree);
  static void drive(int dir);
  
  static PulseTracker startMeasurement(int needed);
  static int getPulses();
  
  static void calculateNewPosition(int degreeTurned, int pulsesDriven);
  static double* calculateBaseDirection();
  static void resetPosition();
};

#endif // Driving_h
