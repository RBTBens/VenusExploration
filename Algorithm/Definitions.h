// Global code identifiers
#define __DEBUG

// Specific debug defines
#ifdef __DEBUG
#define __DEBUG_DRIVING
#endif // __DEBUG

// Arduino library
#include "Arduino.h"

// Arduino pin setup ids
#define ID_LEFTENCODER 7
#define ID_RIGHTENCODER 8
#define ID_UDSSENSOR 9
#define ID_GRIPPERSERVO 10
#define ID_UDSSERVO 11
#define ID_LEFTSERVO 12
#define ID_RIGHTSERVO 13

// Servo driving speeds
#define LEFT_FORWARD 106
#define LEFT_REVERSE 74
#define RIGHT_FORWARD 74
#define RIGHT_REVERSE 106
#define SERVO_NEUTRAL 92

// Driving constants
#define DEGREE_PER_PULSE 13.11

// UDS constants
#define UDS_SOUNDSPEED 58
