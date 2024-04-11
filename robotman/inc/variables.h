#ifndef VARIABLES_H
#define VARIABLES_H

// All includes and global definitions should be here

#include <Adafruit_PWMServoDriver.h>
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include <Wire.h>

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps612.h"

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define MPU 0x68

// Set an error value for int16_t
#define INT16_ERROR_VALUE INT16_MIN

#define SMALL_STRING_LENGTH 16

extern MPU6050 mpu;

/* =========================================================================
 NOTE: In addition to connection 3.3v, GND, SDA, and SCL, this sketch
 depends on the MPU-6050's INT pin being connected to the Arduino's
 external interrupt #0 pin. On the Arduino Uno and Mega 2560, this is
 digital I/O pin 2.
 ========================================================================= */

// Assign each joint with a unique port address
enum Bodyparts : int {
    LEFT_SHOULDER,
    RIGHT_SHOULDER,
    LEFT_ARM,
    RIGHT_ARM,
    LEFT_WRIST,
    RIGHT_WRIST,
    LEFT_HIP,
    RIGHT_HIP,
    LEFT_THIGH,
    RIGHT_THIGH,
    LEFT_KNEE,
    RIGHT_KNEE,
    LEFT_ANKLE,
    RIGHT_ANKLE,
    LEFT_FOOT,
    RIGHT_FOOT,
    HEAD,  // Not assigned : Not enough pins on the UNO motor shield
    BODYPART_COUNT
};

extern char Bodypart_name[BODYPART_COUNT][SMALL_STRING_LENGTH];

enum JointDefintions : int {
    JOINT_MIN,
    JOINT_MAX,
    JOINT_DIR,
    JOINT_PLACEMENT_COUNT
};

extern int16_t joint_definitions[BODYPART_COUNT][JOINT_PLACEMENT_COUNT];

enum Stances : int {
    STANCE_IDLE,
    STANCE_SQUAT,
    STANCE_INVERSE_SQUAT,
    STANCE_COUNT
};

extern int16_t stance_array[BODYPART_COUNT][STANCE_COUNT];

typedef struct JointCurrentPosition {
    uint16_t actual_position;     // Servos internal position
    int16_t normalized_position;  // -100:100 position
} JointCurrentPosition;

extern JointCurrentPosition joint_current_position[BODYPART_COUNT];

extern int STANCE_CURRENT;
extern bool BALANCE_MODE;
extern bool DEBUG_MODE;

// Create a PWM object from the Adafruit library
extern Adafruit_PWMServoDriver pwm;

// Ultrasonic sensor global variables
extern float ultra_sonic_distance;
extern const byte trigPin;
extern const byte echoPin;

// MPU6050 global variables
extern float temperature;  // Temperature sensor in the the MPU6050

// MPU control/status vars
extern bool dmpReady;           // set true if DMP init was successful
extern uint8_t mpuIntStatus;    // holds actual interrupt status byte from MPU
extern uint8_t devStatus;       // return status after each device operation (0 = success, !0 = error)
extern uint16_t packetSize;     // expected DMP packet size (default is 42 bytes)
extern uint16_t fifoCount;      // count of all bytes currently in FIFO
extern uint8_t fifoBuffer[64];  // FIFO storage buffer

// orientation/motion vars
extern Quaternion q;         // [w, x, y, z]         quaternion container
extern VectorInt16 aa;       // [x, y, z]            accel sensor measurements
extern VectorInt16 gy;       // [x, y, z]            gyro sensor measurements
extern VectorInt16 aaReal;   // [x, y, z]            gravity-free accel sensor measurements
extern VectorInt16 aaWorld;  // [x, y, z]            world-frame accel sensor measurements
extern VectorFloat gravity;  // [x, y, z]            gravity vector
extern float ypr[3];         // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
extern float yaw, pitch, roll;

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

extern volatile bool mpuInterrupt;  // indicates whether MPU interrupt pin has gone high

#endif  // VARIABLES_H