#ifndef Sample_h
#define Sample_h

// The detector
class SampleDetector
{
private:
  int led;
  uint8_t sensor;

public:
  SampleDetector(int led, uint8_t sensor);
  
  void link();
  float read();
};

// The class interface
class Sample
{
private:
  static SampleDetector front;
  static SampleDetector left;
  static SampleDetector right;

public:
  static void setupDetectors();
  static float getValue(SensorPos pos);
};

#endif // Sample_h

