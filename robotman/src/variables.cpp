#include "../inc/variables.h"

MPU6050 mpu(MPU);

char Bodypart_name[BODYPART_COUNT][SMALL_STRING_LENGTH]{
    "LEFT_SHOULDER",
    "RIGHT_SHOULDER",
    "LEFT_ARM",
    "RIGHT_ARM",
    "LEFT_WRIST",
    "RIGHT_WRIST",
    "LEFT_HIP",
    "RIGHT_HIP",
    "LEFT_THIGH",
    "RIGHT_THIGH",
    "LEFT_KNEE",
    "RIGHT_KNEE",
    "LEFT_ANKLE",
    "RIGHT_ANKLE",
    "LEFT_FOOT",
    "RIGHT_FOOT",
    "HEAD"  // Not assigned : Not enough pins on the UNO motor shield
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
    {100, 520, 1}    // HEAD            16
};

int16_t stance_array[BODYPART_COUNT][STANCE_COUNT]{
    {200, 80, 200},   // LEFT_SHOULDER
    {310, 500, 310},  // RIGHT_SHOULDER
    {400, 450, 400},  // LEFT_ARM
    {150, 150, 150},  // RIGHT_ARM
    {300, 120, 300},  // LEFT_WRIST
    {220, 420, 220},  // RIGHT_WRIST
    {240, 240, 240},  // LEFT_HIP
    {260, 260, 260},  // RIGHT_HIP
    {390, 500, 160},  // LEFT_THIGH
    {200, 100, 410},  // RIGHT_THIGH
    {250, 400, 60},   // LEFT_KNEE
    {250, 100, 500},  // RIGHT_KNEE
    {350, 430, 225},  // LEFT_ANKLE
    {340, 340, 500},  // RIGHT_ANKLE
    {300, 300, 325},  // LEFT_FOOT
    {370, 370, 370},  // RIGHT_FOOT
    {255, 255, 255}   // HEAD
};

JointCurrentPosition joint_current_position[BODYPART_COUNT];

int STANCE_CURRENT{0};
bool BALANCE_MODE{false};
bool DEBUG_MODE{false};

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
