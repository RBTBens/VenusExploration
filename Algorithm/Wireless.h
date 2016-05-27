#ifndef Wireless_h
#define Wireless_h

// The class interface
class Wireless
{
private:
  static int NumberContainer[VARIABLE_COUNT];

public:
  static void initialize();
  static void setVariable(byte id, int value);
  static int getVariable(byte id);
};

#endif // Wireless_h

