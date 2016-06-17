#ifndef Gripper_h
#define Gripper_h

// The class interface
class Gripper
{
private:
  static Servo gripperServo;

public:
  static void initialize();
  static void open();
  static void close();
  static void idle();
  static void clapYourHands();
};

#endif // Gripper_h

