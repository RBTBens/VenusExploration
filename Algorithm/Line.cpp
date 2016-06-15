#include "Definitions.h"
#include "Line.h"

// Trigger function
void Line::trigger(byte pin)
{
  // Read the state of the sensors (active-low so take the inverse)
  bool right = !digitalRead(2);
  bool left = !digitalRead(3);

#ifdef __DEBUG_LINE
  Serial.print("Triggered on pin: ");
  Serial.println(pin);
  Serial.print("Right(2): ");
  Serial.println(right);
  Serial.print("Left(3): ");
  Serial.println(left);
#endif // __DEBUG_LINE

  // Right line sensor triggered
  if (right && !left)
  {
    setSubState(SUB_RIGHT_LINE);
  }

  // Left line sensor triggered
  else if (!right && left)
  {
    setSubState(SUB_LEFT_LINE);
  }

  // Both line sensors triggered at once
  // Choose a random rotation direction
  else if (right && left)
  {
    setSubState(random(2) == 0 ? SUB_RIGHT_LINE : SUB_LEFT_LINE);
  }
}
