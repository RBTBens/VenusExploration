#ifndef UDS_h
#define UDS_h
#include "Arduino.h"
#include <Servo.h>

// Compile-time numbers
#define ID_UDSSERVO 11

// The class interface
class UDS
{
public:
  UDS();
  ~UDS();

  void initialize();
};

#endif // UDS_h
