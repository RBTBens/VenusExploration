#include "Definitions.h"
#include "UDS.h"

// Initialize variables
Servo UDS::udsServo;
int UDS::currentDegree = 0;
int UDS::sweepEnd = 0;
byte UDS::sweepType = 0;
long UDS::sweepTimer = 0;
char UDS::sweepDirection = 1;

// Initialization function
void UDS::initialize()
{
  // Attach the servo
  udsServo.attach(ID_UDSSERVO);
  udsServo.write(currentDegree);
}

// Think function
void UDS::think()
{
  // Check if we can think already
  if (sweepTimer > millis())
    return;
  else
    sweepTimer = millis() + UDS_SWEEP_DELAY;
  
  // Constant sweeping
  if (sweepType == 1)
  {
    if (currentDegree <= UDS_SWEEP_MIN)
      sweepDirection = 1;
    else if (currentDegree >= UDS_SWEEP_MAX)
      sweepDirection = -1;
    
    // Do a measurement every 5 degrees
    if (currentDegree % 5 == 0)
    {
#ifdef __DEBUG_UDS
      Serial.print(normalizeDegree(currentDegree));
      Serial.print(" -> ");
      Serial.println(timeToCentimeters(readDistance()));
#endif // __DEBUG_UDS
    }

    // Change value and apply to the servo
    currentDegree += sweepDirection;
    udsServo.write(currentDegree);
  }
  else if (sweepType == 2)
  {
    // To-Do: Do a single sweep
  }
}

// Set sweep mode
void UDS::setSweep(byte type)
{
  sweepType = type;

#ifdef __DEBUG_UDS
  Serial.print("Set sweep type to: ");
  Serial.println(type);
#endif // __DEBUG_UDS
}

// Normalizes degree
int UDS::normalizeDegree(int degree)
{
  return -degree + UDS_ANGLE_BASE;
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
  int nDiff = abs(currentDegree - degree);
  currentDegree = degree;
  
  // Wait to make sure the servo is done turning
  delay(UDS_ROTATETIME + nDiff * UDS_TIMEPERDEGREE);
  
  // Wait for the pulse and convert to centimeters
  return timeToCentimeters(readDistance());
}

// Returns an array of distances of a range of degrees
long* UDS::distanceInRangeOfDegrees(int degreeBegin, int degreeEnd)
{
  //currentDegree = degreeBegin;
  //sweepEnd = degreeEnd;
  //sweepTimer = millis() + UDS_DELAYSCAN;
  
  return NULL;
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

