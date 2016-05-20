#ifndef UDS_h
#define UDS_h
#include <Servo.h>

// Compile-time numbers
#define ID_UDSSERVO 11
#define ID_UDS 9
#define UDS_SOUNDSPEED 58

// The class interface
class UDS
{
public:
  UDS();

  void initialize();

  long timeToCentimeters(long timeDifference);
  long distanceOfDegree(int degree);
  long* distanceOfRangeOfDegrees(int degreeBegin, int degreeEnd);
};

#endif // UDS_h

