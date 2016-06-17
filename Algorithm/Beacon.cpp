#include "Definitions.h"
#include "Beacon.h"

// Variables
int Beacon::intensityValues[BASE_FULL_CIRCLE_PULSES];

// Checking function
bool Beacon::seeHome()
{
  // To-Do: Make this threshold make sense
  return readValue() >= BEACON_HOME_THRESHOLD;
}

// Reading function
int Beacon::readValue(int save)
{
  // To-Do: Do we need to use the ambient value checking here as well?
  int val = analogRead(IR_BEACON_SENSOR);

  // Check if we need to save, also make sure we're not going out of bounds
  if (save >= 0 && save < BASE_FULL_CIRCLE_PULSES)
    intensityValues[save] = val;
  
  return val;
}

// Returns the maximum value in the intensity array
int Beacon::getMaximumValue()
{
  // Set some tracking vars
  int maxVal = 0;
  
  // Loop over the collected data array to find the largest value
  for (int i = 0; i < BASE_FULL_CIRCLE_PULSES; i++)
  {
    if (intensityValues[i] > maxVal)
      maxVal = intensityValues[i];
  }
  
  return maxVal;
}

// Dump data of circle
void Beacon::dumpValues()
{
  for (int i = 0; i < BASE_FULL_CIRCLE_PULSES; i++)
  {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(intensityValues[i]);
  }
}

