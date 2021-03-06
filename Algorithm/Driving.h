#ifndef Driving_h
#define Driving_h

// Pulse tracking
class PulseTracker
{
private:
  int startLeft;
  int startRight;
  int neededPulses;
  
public:
  PulseTracker();
  PulseTracker(int l, int r, int n);
  
  int getPulses();
  bool hasCompleted(int n = 0);
  void reset();
};

// The class interface
class Driving
{
private:
  static Servo leftWheel;
  static Servo rightWheel;

  static volatile int leftPulses;
  static volatile int rightPulses;
  
  static double relativeXPosition;
  static double relativeYPosition;
  static double relativeOrientation;

  static DrivingStatus drivingStatus;
  static int lastLeftPulses;
  static int lastRightPulses;
  
public:
  static void initialize();
  static void trigger(byte pin);
  static void drive(int dir);
  static void rotate(int dir = 0);
  
  static PulseTracker startMeasurement(int needed);
  static int getPulses();

  static void map();
  static void calculateNewPosition(double degreeTurned, double pulsesDriven);
  static double* calculateBaseDirection();
  static void resetPositionForward();
  static void resetPositionReverse();
};

#endif // Driving_h
