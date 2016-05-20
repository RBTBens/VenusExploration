#ifndef UDS_h
#define UDS_h
#include <Servo.h>

// The class interface
class UDS
{
private:
  static UDS* instance;

public:
  UDS();

  void initialize();

  long timeToCentimeters(long timeDifference);
  long distanceAtDegree(int degree);
  long* distanceOfRangeOfDegrees(int degreeBegin, int degreeEnd);
  unsigned long readDistance();

  static UDS* getInstance();
};

#endif // UDS_h
