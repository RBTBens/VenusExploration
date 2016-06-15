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
    Serial.println("Jo wordt kut right");
    setSubState(SUB_RIGHT_LINE);
    Serial.println("Jo was toch niet kut right");
  }

  // Left line sensor triggered
  else if (!right && left)
  {
    Serial.println("Jo wordt kut left");
    setSubState(SUB_LEFT_LINE);
    Serial.println("Jo was toch niet kut left");
  }

  // Both line sensors triggered at once
  else if (right && left)
  {
    // Choose a random rotation direction
    long randomNumber = random(2);
    if (randomNumber == 0)
      setSubState(SUB_RIGHT_LINE);
    else
      setSubState(SUB_LEFT_LINE);
  }
}
