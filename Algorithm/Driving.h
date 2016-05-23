#ifndef Driving_h
#define Driving_h
#include <Servo.h>

// The class interface
class Driving
{
private:
  static Driving* instance;
  
public:
  Driving();

  void initialize();
  void trigger(byte pin);
  void rotate(float degree);
  void drive(int dir);
  void calculateNewPosition(int degreeTurned, int pulsesDriven);
  void resetPosition();

  static Driving* getInstance();
};

#endif // Driving_h
