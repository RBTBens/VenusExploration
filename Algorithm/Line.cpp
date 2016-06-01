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

  // Right line sensor triggered
  if (pin == 2)
  {
    Driving::drive(0);
    Driving::rotate(4 * DEGREE_PER_PULSE);
    Driving::drive(1);
    //later add mapping
  }

  // Left line sensor triggered
  if (pin == 3)
  {
    Driving::drive(0);
    Driving::rotate(-4 * DEGREE_PER_PULSE);
    Driving::drive(1);
    //later add mapping
  }
}

