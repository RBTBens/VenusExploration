#ifndef UDS_h
#define UDS_h

// The class interface
class UDS
{
private:
  static Servo udsServo;
  static int currentDegree;
  static int scanStep;
  static unsigned long scanTime;

public:
  static void initialize();
  static int normalizeDegree(int degree);
  static long timeToCentimeters(long timeDifference);
  static long sweepForShortestDistance();
  static long pollForDistance();
  static long distanceAtDegree(int degree, bool single = false);
  static unsigned long readDistance();
};

#endif // UDS_h
