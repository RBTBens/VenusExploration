#ifndef UDS_h
#define UDS_h
#include <Servo.h>

// Compile-time numbers
#define ID_UDSSERVO 11
#define ID_UDS 9

// The class interface
class UDS
{
public:
  UDS();

  void initialize();

  long timeDifferenceToCentimeters(long timeDifference);
  long distanceOfDegree(int degree);
  long* distanceOfRangeOfDegrees(int degreeBegin, int degreeEnd);
};

#endif // UDS_h
