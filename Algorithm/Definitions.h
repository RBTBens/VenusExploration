// Global code identifiers
#define __DEBUG

// Specific debug defines
#ifdef __DEBUG
#define __DEBUG_DRIVING
#endif // __DEBUG

// Libraries
#include "Arduino.h"
#include "avr/interrupt.h"
#include "Servo.h"

// Arduino pin setup ids
#define ID_RIGHTLINESENSOR 2  //PD2 PCINT18
#define ID_LEFTLINESENSOR 3   //PD3 PCINT19
#define ID_LEFTENCODER 7      //PD7 PCINT23
#define ID_RIGHTENCODER 8     //PB0 PCINT0
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
#define SERVO_NEUTRAL 90

// Driving constants
#define DEGREE_PER_PULSE 13.11

// UDS constants
#define UDS_SOUNDSPEED 58
#define UDS_ROTATETIME 100
#define UDS_TIMEPERDEGREE 6
