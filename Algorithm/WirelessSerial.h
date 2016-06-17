#ifndef WirelessSerial_h
#define WirelessSerial_h

// The class interface
class Wireless
{  
private:
  static int NumberContainer[VAR_COUNT];
  static int RemoteContainer[VAR_COUNT];

  static bool isReading;
  static byte writePointer;
  static byte packetId;
  static byte readBuffer[BUFFER_LENGTH];

public:
  static void open();
  static void read();
  
  static void setVariable(byte id, int value, bool submit = false);
  static int getVariable(byte id, bool remote = false);
  static void onReceiveMsg(byte buf[]);
};

#endif // WirelessSerial_h

