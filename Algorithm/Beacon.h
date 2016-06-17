#ifndef Beacon_h
#define Beacon_h

// The class interface
class Beacon
{
private:
  static int intensityValues[BASE_FULL_CIRCLE_PULSES];
  
public:
  static bool seeHome();
  static int readValue(int save = -1);
  static int getMaximumValue();
  static void dumpValues();
};

#endif // Beacon_h

