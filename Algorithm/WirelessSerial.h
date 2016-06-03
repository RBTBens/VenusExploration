#ifndef WirelessSerial_h
#define WirelessSerial_h

// The class interface
class Wireless
{
private:
  static int NumberContainer[VARIABLE_COUNT];
  static int RemoteContainer[VARIABLE_COUNT];

public:
  static void initialize();
  static void setVariable(byte id, int value, bool remote = false);
  static int getVariable(byte id, bool remote = false);
  static void onReceiveMsg(byte packet, byte id, int value);
};

#endif // WirelessSerial_h

