#include "Definitions.h"
#include "Line.h"

// Checking function
bool Line::isOverLine()
{
  return digitalRead(ID_LEFTLINESENSOR) || digitalRead(ID_RIGHTLINESENSOR);
}

// Trigger function
void Line::trigger(byte pin)
{
#ifdef __DEBUG_LINE
  Serial.print("Triggered on pin: ");
  Serial.println(pin);
#endif // __DEBUG_LINE

  bool right = digitalRead(2);
  bool left = digitalRead(3);

  // Right line sensor triggered
  if (right && !left)
  {
    Driving::addCallback(REVERSE_PULSES, onLeftReverse);
    Driving::drive(-1);
  }

  // Left line sensor triggered
  else if (!right && left)
  {
    Driving::addCallback(REVERSE_PULSES, onRightReverse);
    Driving::drive(-1);
  }
}

// Left callback
void Line::onLeftReverse()
{
  Driving::addCallback(-1, onRotateFinish);
  Driving::rotate(-3 * DEGREE_PER_PULSE);
}

// Right callback
void Line::onRightReverse()
{
  Driving::addCallback(-1, onRotateFinish);
  Driving::rotate(3 * DEGREE_PER_PULSE);
}

// Rotate callback
void Line::onRotateFinish()
{
  Serial.println("Finish!");
  Driving::drive(1);
}
