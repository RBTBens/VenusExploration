#include "Definitions.h"
#include "Line.h"

// Checking function
bool Line::isOverLine()
{
  return digitalRead(ID_LEFTLINESENSOR) == HIGH || digitalRead(ID_RIGHTLINESENSOR) == HIGH;
}

// Trigger function
void Line::trigger(byte pin)
{
#ifdef __DEBUG_LINE
  Serial.print("Triggered on pin: ");
  Serial.println(pin);
#endif // __DEBUG_LINE
}

