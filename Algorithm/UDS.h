#ifndef UDS_h
#define UDS_h
#include <Servo.h>

// The class interface
class UDS
{
private:
  static Servo udsServo;
  static int currentDegree;
  static int sweepEnd;
  static byte sweepType;
  static long sweepTimer;
  static char sweepDirection;

public:
  static void initialize();
  static void think();
  static void setSweep(byte type);

  static int normalizeDegree(int degree);
  static long timeToCentimeters(long timeDifference);
  static long distanceAtDegree(int degree);
  static long* distanceInRangeOfDegrees(int degreeBegin, int degreeEnd);
  static unsigned long readDistance();
};

#endif // UDS_h
