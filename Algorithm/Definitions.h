// Required libraries
#include "Arduino.h"
#include "avr/interrupt.h"
#include "Servo.h"

// Global code identifiers
//#define __DEBUG
//#define __IR_SIDE_CHECK

// Specific debug defines
#ifdef __DEBUG
#define __DEBUG_SERIAL
#define __DEBUG_DRIVING
//#define __DEBUG_DRIVING_TRIGGER
//#define __DEBUG_DRIVING_POSITION
#define __DEBUG_UDS
#define __DEBUG_LINE
#define __DEBUG_BEACON
#endif // __DEBUG

// Arduino pin setup ids
#define IR_LEFT_SENSOR A1
#define IR_RIGHT_SENSOR A2
#define IR_BEACON_SENSOR A3
#define IR_FRONT_SENSOR A4
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

// General constants
#define VENUS_SAMPLE_COUNT 5
#define LAST_ROBOT_WAIT_TIME 5000

// Servo driving speeds
#define LEFT_FORWARD 106
#define LEFT_REVERSE 74
#define RIGHT_FORWARD 74
#define RIGHT_REVERSE 106
#define SERVO_NEUTRAL 90

// Driving constants
#define DEGREE_PER_PULSE 13.11
#define COLLISION_REVERSE_PULSES 3
#define COLLISION_ROTATION_PULSES 6
#define COLLISION_RANDOM_ROTATION_PULSES 4
#define UDS_ROTATION_PULSES 6
#define UDS_REVERSE_PULSES 5
#define FIRST_OFF_BASE_PULSES 24
#define LAST_OFF_BASE_PULSES 48
#define PICKUP_PULSES 6
#define PICKUP_REVERSE_PULSES 12
#define SIDE_IR_SAMPLE_ROTATE 7
#define BASE_FULL_CIRCLE_PULSES 28
#define BASE_DRIVE_UP 40
#define BASE_DRIVE_DOWN 60

// UDS constants
#define UDS_SOUNDSPEED 58
#define UDS_ROTATETIME 15
#define UDS_TIMEPERDEGREE 6
#define UDS_ANGLE_BASE 80
#define UDS_SWEEP_DELAY 100
#define UDS_SWEEP_MIN 60
#define UDS_SWEEP_MAX 100
#define UDS_ROBOT_FRONT_DISTANCE 40
#define UDS_COLLISION_DISTANCE 20
#define UDS_COLLISION_REVERSE_DISTANCE 10
#define UDS_SAMPLE_DROP_DISTANCE 25
#define UDS_MAX_DISTANCE 1000
#define UDS_NOSAMPLE_DISTANCE 18
#define UDS_BASE_WALL_DISTANCE 12

// Gripper values
#define GRIPPER_ANG_MIN 0
#define GRIPPER_ANG_MAX 120
#define GRIPPER_ANG_IDLE 75
#define GRIPPER_CLAP_MIN 60
#define GRIPPER_CLAP_DELAY 2
#define GRIPPER_DELAY 1000

// Sample IR
#define IR_ACCURACY 5.0
#define IR_DELAY 1
#define IR_FRONT_THRESHOLD 500
#define IR_SIDE_THRESHOLD 100
#define IR_PICKUP_THRESHOLD 900

// Wireless defines
#define BUFFER_LENGTH 16
#define PACKET_ENDING 93
#define PACKET_OPENING 94
#define PACKET_SYNC 1

// Beacon defines
#define BEACON_ROTATE_MAX 84
#define BEACON_HOME_THRESHOLD 80

// Status enum
enum RobotStatus
{
  DEBUG = -1,
  START_ON_BASE = 0,
  SEARCHING_SAMPLE = 1,
  VERIFYING_SAMPLE = 2,
  PICKING_UP_SAMPLE = 3,
  SEARCHING_BASE = 4,
  DRIVING_UP_BASE = 5,
  DROPPING_SAMPLE = 6,
  DRIVING_DOWN_BASE = 7,
  DONE = 8
};

// Sub statuses
enum RobotSubStatus
{
  SUB_START = 0,
  SUB_DRIVING_COMMAND = 1,
  SUB_DRIVING = 2,
  SUB_ROTATING_COMMAND = 3,
  SUB_ROTATING = 4,
  SUB_CLAP = 5,
  SUB_RIGHT_LINE = 6,
  SUB_LEFT_LINE = 7,
  SUB_REVERSE_COMMAND = 8,
  SUB_REVERSE = 9,
  SUB_LOCKBASE_COMMAND = 10,
  SUB_LOCKBASE = 11
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
  VAR_SAMPLES = 1,
  VAR_COUNT = 2
};

// Driving enum
enum DrivingStatus
{
  DRIVING_NEUTRAL = 0,
  DRIVING_FORWARD = 1,
  DRIVING_REVERSE = 2,
  DRIVING_ROTATING_CW = 3,
  DRIVING_ROTATING_CCW = 4
};

// Global classes
#include "WirelessSerial.h"

// Global types
typedef RobotSubStatus (*subStateGetter)(void);
typedef void (*subStateSetter)(RobotSubStatus);
