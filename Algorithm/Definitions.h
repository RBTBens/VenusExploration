// Global code identifiers
#define __DEBUG

// Specific debug defines
#ifdef __DEBUG
#define __DEBUG_DRIVING
#define __DEBUG_UDS
#define __DEBUG_LINE
#endif // __DEBUG

// Libraries
#include "Arduino.h"
#include "avr/interrupt.h"
#include "Servo.h"

// Arduino pin setup ids
#define IR_FRONT_SENSOR A0
#define IR_LEFT_SENSOR A1
#define IR_RIGHT_SENSOR A2
#define IR_BEACON_SENSOR A3
#define ID_RIGHTLINESENSOR 2  // PD2 PCINT18
#define ID_LEFTLINESENSOR 3   // PD3 PCINT19
#define ID_FRONTLED 4
#define ID_LEFTLED 5
#define ID_RIGHTLED 6
#define ID_LEFTENCODER 7      // PD7 PCINT23
#define ID_RIGHTENCODER 8     // PB0 PCINT0
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
#define REVERSE_PULSES 5

// UDS constants
#define UDS_SOUNDSPEED 58
#define UDS_ROTATETIME 15
#define UDS_TIMEPERDEGREE 6
#define UDS_ANGLE_BASE 80
#define UDS_SWEEP_DELAY 8
#define UDS_SWEEP_MIN 40
#define UDS_SWEEP_MAX 120

// Sample IR
#define IR_ACCURACY 5.0
#define IR_DELAY 1

// Wireless defines
#define VARIABLE_COUNT 2
#define PACKET_SYNC 1
#define PACKET_REQUEST 2
#define PACKET_INCREASE 3
#define PACKET_DECREASE 4

// Status enum
enum RobotStatus
{
  START_ON_BASE = 0,
  SEARCHING_SAMPLE = 1,
  PICKING_UP_SAMPLE = 2,
  SEARCHING_BASE = 3,
  DRIVING_UP_BASE = 4,
  DROPPING_SAMPLE = 5,
  DRIVING_DOWN_BASE = 6,
  DONE = 7
};

// Sub statuses
enum RobotSubStatus
{
  SUB_DRIVING_COMMAND = 1,
  SUB_DRIVING = 2,
  SUB_ROTATING_COMMAND = 3,
  SUB_ROTATING = 4
};

// Location enum
enum SensorPos
{
  POS_FRONT = 1,
  POS_LEFT = 2,
  POS_RIGHT = 3
};

// Wireless enum
enum WirelessVar
{
  VAR_STATUS = 0,
  VAR_SAMPLES = 1
};

// Global classes
#include "WirelessSerial.h"
