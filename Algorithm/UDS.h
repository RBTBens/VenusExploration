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
  static long timeToCentimeters(long timeDifference);
  static unsigned long readDistance();
  static long pollForDistance();
  static long distanceAtDegree(int degree);
};

#endif // UDS_h
