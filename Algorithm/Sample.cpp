#include "Definitions.h"
#include "Sample.h"

// Variables
SampleDetector Sample::front(ID_FRONTLED, IR_FRONT_SENSOR);
SampleDetector Sample::left(ID_LEFTLED, IR_LEFT_SENSOR);
SampleDetector Sample::right(ID_RIGHTLED, IR_RIGHT_SENSOR);

// Initialization function
void Sample::setupDetectors()
{
  front.link();
  left.link();
  right.link();
}

// Read the value from the static class
float Sample::getValue(SensorPos pos)
{
  // Depending on the SensorPos enum select an object to read from
  if (pos == POS_FRONT)
    return front.read();
  else if (pos == POS_LEFT)
    return left.read();
  else if (pos == POS_RIGHT)
    return right.read();
  else
    return 0;
}

// Sample Detector class
SampleDetector::SampleDetector(int led, uint8_t sensor)
{
  this->led = led;
  this->sensor = sensor;
}

// Connect the pins
void SampleDetector::link()
{
  pinMode(this->led, OUTPUT);
  digitalWrite(this->led, LOW);
  pinMode(this->sensor, INPUT);
}

// Read values
float SampleDetector::read()
{
  float distance = 0;

  // Perform measurement a couple times and get the average
  for (int i = 0; i < IR_ACCURACY; i++)
  {
    digitalWrite(this->led, LOW);
    delay(IR_DELAY);
    int ambientValue = analogRead(this->sensor);
    
    digitalWrite(this->led, HIGH);
    delay(IR_DELAY);
    int sampleValue = analogRead(this->sensor);
    
    distance += sampleValue - ambientValue;
  }
  
  return distance / IR_ACCURACY;
}
