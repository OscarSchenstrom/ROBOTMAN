#include "../inc/variables.h"

MPU6050 mpu(MPU);

char Bodypart_name[BODYPART_COUNT][SMALL_STRING_LENGTH]{
    "L_SHOULDER",
    "R_SHOULDER",
    "L_ARM",
    "R_ARM",
    "L_WRIST",
    "R_WRIST",
    "L_HIP",
    "R_HIP",
    "L_THIGH",
    "R_THIGH",
    "L_KNEE",
    "R_KNEE",
    "L_ANKLE",
    "R_ANKLE",
    "L_FOOT",
    "R_FOOT",
};

int16_t joint_definitions[BODYPART_COUNT][JOINT_PLACEMENT_COUNT]{
    {100, 500, -1},  // LEFT_SHOULDER   0
    {100, 500, 1},   // RIGHT_SHOULDER  1
    {103, 450, -1},  // LEFT_ARM        2
    {103, 450, 1},   // RIGHT_ARM       3
    {120, 410, -1},  // LEFT_WRIST      4
    {170, 420, 1},   // RIGHT_WRIST     5
    {110, 500, 1},   // LEFT_HIP        6
    {100, 500, -1},  // RIGHT_HIP       7
    {260, 530, 1},   // LEFT_THIGH      8
    {70, 410, -1},   // RIGHT_THIGH     9
    {100, 500, -1},  // LEFT_KNEE       10
    {100, 500, 1},   // RIGHT_KNEE      11
    {280, 500, 1},   // LEFT_ANKLE      12
    {300, 520, -1},  // RIGHT_ANKLE     13
    {200, 400, 1},   // LEFT_FOOT       14
    {200, 500, -1},  // RIGHT_FOOT      15 
    //{100, 520, 1}    // HEAD            16
};

int16_t stance_array[BODYPART_COUNT][STANCE_COUNT]{
    {200, 80, 200},   // LEFT_SHOULDER
    {310, 500, 310},  // RIGHT_SHOULDER
    {400, 450, 400},  // LEFT_ARM
    {150, 150, 150},  // RIGHT_ARM
    {300, 120, 300},  // LEFT_WRIST
    {220, 420, 220},  // RIGHT_WRIST
    {250, 240, 240},  // LEFT_HIP
    {300, 260, 260},  // RIGHT_HIP 
    {435, 500, 390},  // LEFT_THIGH
    {155, 100, 200},  // RIGHT_THIGH
    {300, 400, 250},  // LEFT_KNEE
    {200, 100, 250},  // RIGHT_KNEE
    {390, 430, 350},  // LEFT_ANKLE
    {355, 340, 340},  // RIGHT_ANKLE
    {300, 300, 300},  // LEFT_FOOT
    {370, 370, 370},  // RIGHT_FOOT 
    //{255, 255, 255}   // HEAD
};

JointCurrentPosition joint_current_position[BODYPART_COUNT];

Stance STANCE_CURRENT{STANCE_IDLE};
bool BALANCE_MODE{false};
bool DEBUG_MODE{false};
bool MENY_LIST{false};

// Create a PWM object from the Adafruit library
Adafruit_PWMServoDriver pwm{Adafruit_PWMServoDriver()};

// Ultrasonic sensor global variables
float ultra_sonic_distance;
const byte trigPin{10};
const byte echoPin{9};

// MPU6050 global variables
float temperature;  // Temperature sensor in the the MPU6050

// MPU control/status vars
bool dmpReady{false};    // set true if DMP init was successful
uint8_t mpuIntStatus;    // holds actual interrupt status byte from MPU
uint8_t devStatus{-1};   // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;     // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;      // count of all bytes currently in FIFO
uint8_t fifoBuffer[64];  // FIFO storage buffer

// orientation/motion vars
Quaternion q;         // [w, x, y, z]         quaternion container
VectorInt16 aa;       // [x, y, z]            accel sensor measurements
VectorInt16 gy;       // [x, y, z]            gyro sensor measurements
VectorInt16 aaReal;   // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;  // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;  // [x, y, z]            gravity vector
float ypr[3];         // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
float yaw, pitch, roll;

volatile bool mpuInterrupt{false};  // indicates whether MPU interrupt pin has gone high
