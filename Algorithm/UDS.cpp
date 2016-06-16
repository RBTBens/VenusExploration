#include "Definitions.h"
#include "UDS.h"

// Initialize variables
Servo UDS::udsServo;
int UDS::currentDegree = UDS_ANGLE_BASE;
int UDS::scanStep = 0;
unsigned long UDS::scanTime = 0;

// Initialization function
void UDS::initialize()
{
  // Attach the servo
  udsServo.attach(ID_UDSSERVO);
  udsServo.write(currentDegree);
}

// Converts time difference to centimeters
long UDS::timeToCentimeters(long timed)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter
  // The wave travels back and forth so the time must be devided by two
  return timed / UDS_SOUNDSPEED;
}

// Send out a sound wave and measure it
unsigned long UDS::readDistance()
{
  // Send out a sound wave
  pinMode(ID_UDSSENSOR, OUTPUT);
  digitalWrite(ID_UDSSENSOR, LOW);
  delayMicroseconds(2);
  digitalWrite(ID_UDSSENSOR, HIGH);
  delayMicroseconds(5);
  digitalWrite(ID_UDSSENSOR, LOW);

  // Use the same pin to detect the soundwave aggain
  pinMode(ID_UDSSENSOR, INPUT);

  // Time it
  return pulseIn(ID_UDSSENSOR, HIGH);
}

// Polls the UDS for data
long UDS::pollForDistance()
{
  unsigned long ms = millis();
  if (ms > scanTime)
  {
    if (scanStep == 0)
    {
      udsServo.write(UDS_ANGLE_BASE);
      scanTime = ms + UDS_SWEEP_DELAY;
      scanStep = 1;
    }
    else if (scanStep == 1)
    {
      udsServo.write(UDS_SWEEP_MAX);
      scanTime = ms + UDS_SWEEP_DELAY;
      scanStep = 2;
    }
    else if (scanStep == 2)
    {
      udsServo.write(UDS_ANGLE_BASE);
      scanTime = ms + UDS_SWEEP_DELAY;
      scanStep = 3;
    }
    else if (scanStep == 3)
    {
      udsServo.write(UDS_SWEEP_MIN);
      scanTime = ms + UDS_SWEEP_DELAY;
      scanStep = 0;
    }

    return timeToCentimeters(readDistance());
  }
  
  return UDS_MAX_DISTANCE;
}

// Returns the distance of one specific degree
long UDS::distanceAtDegree(int degree)
{
  // Turn servo to the desired degree
  udsServo.write(degree);

  // Get the difference and update it
  int nDiff = abs(currentDegree - degree);
  currentDegree = degree;
  
  // Wait to make sure the servo is done turning
  delay(UDS_ROTATETIME + nDiff * UDS_TIMEPERDEGREE);
  
  // Wait for the pulse and convert to centimeters
  return timeToCentimeters(readDistance());
}

