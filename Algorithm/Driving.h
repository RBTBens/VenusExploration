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

  static Driving* getInstance();
};

#endif // Driving_h
