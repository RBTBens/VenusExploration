#include "Definitions.h"
#include "UDS.h"

// Create Servo object
Servo udsServo;
int nCurrentDegree = 0;

// Set a null pointer on the instance
UDS* UDS::instance = NULL;

// Constructor
UDS::UDS()
{
  UDS::instance = this;
}

// Instance getter
UDS* UDS::getInstance()
{
  return UDS::instance;
}

// Initialization function
void UDS::initialize()
{
  // Attach the servo
  udsServo.attach(ID_UDSSERVO);
  udsServo.write(nCurrentDegree);
}

// Converts time difference to centimeters
long UDS::timeToCentimeters(long timed)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter
  // The wave travels back and forth so the time must be devided by two
  return timed / UDS_SOUNDSPEED;
}

// Returns the distance of one specific degree
long UDS::distanceAtDegree(int degree)
{
  // Turn servo to the desired degree
  udsServo.write(degree);

  // Get the difference and update it
  int nDiff = abs(nCurrentDegree - degree);
  nCurrentDegree = degree;
  
  // Wait to make sure the servo is done turning
  delay(UDS_ROTATETIME + nDiff * UDS_TIMEPERDEGREE);
  
  // Wait for the pulse and convert to centimeters
  return timeToCentimeters(readDistance());
}

// Returns an array of distances of a range of degrees
long* UDS::distanceOfRangeOfDegrees(int degreeBegin, int degreeEnd)
{
  int arrayLength = abs(degreeEnd - degreeBegin) + 1;
  long* output = (long*)malloc(arrayLength * sizeof(long));
  
  for(int i = degreeBegin; i < degreeEnd + 1; i++)
  {
    output[i - degreeBegin] = distanceAtDegree(i);
  }

  return output;
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

