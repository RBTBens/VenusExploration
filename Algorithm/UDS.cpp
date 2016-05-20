#include "Definitions.h"
#include "UDS.h"

// Create Servo object
Servo udsServo;

// Constructor
UDS::UDS()
{
  
}

// Initialization function
void UDS::initialize()
{
  // Attach the servo
  udsServo.attach(ID_UDSSERVO);
}

// Converts time difference to centimeters
long UDS::timeDifferenceToCentimeters(long timeDifference)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter
  // The wave travels back and forth so the time must be devided by two
  return timeDifference / 58;
}

// Returns the distance of one specific degree
long UDS::distanceOfDegree(int degree)
{
  long timeDifference;
  
  // Turn servo to the desired degree
  udsServo.write(degree);
  // Wait to make sure the servo is done turning
  delay(15);
  
  // Send out a sound wave
  pinMode(ID_UDS, OUTPUT);
  digitalWrite(ID_UDS, LOW);
  delayMicroseconds(2);
  digitalWrite(ID_UDS, HIGH);
  delayMicroseconds(5);
  digitalWrite(ID_UDS, LOW);

  // Use the same pin to detect the soundwave aggain
  pinMode(ID_UDS, INPUT);
  timeDifference = pulseIn(ID_UDS, HIGH);

  return timeDifferenceToCentimeters(timeDifference);
}

// Returns an array of distances of a range of degrees
long* UDS::distanceOfRangeOfDegrees(int degreeBegin, int degreeEnd)
{
  int arrayLength = degreeEnd - degreeBegin;
  long* output = (long*)malloc(arrayLength * sizeof(long));
  
  for(int i = degreeBegin; i < degreeEnd; i++)
  {
    output[i - degreeBegin] = distanceOfDegree(i);
  }

  return output;
}


