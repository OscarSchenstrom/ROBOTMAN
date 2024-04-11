#include "../inc/mpu.h"

#include "../inc/serial_communication.h"
#include "../inc/variables.h"

// All functions regarding the MPU6050 accelerometer/gyroscope/thermostat

void dmpDataReady() {
    mpuInterrupt = true;
}

bool calibrate_mpu() {
    if (devStatus != 0) {
        Serial.println("Can't calibrate MPU if not initialized");
        return false;
    }
    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.setDMPEnabled(false);
    mpu.CalibrateAccel(6);  // 6
    mpu.CalibrateGyro(6);   // 6
    Serial.println();
    mpu.PrintActiveOffsets();
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    Serial.println(F(")..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();

    return true;
}

// Initialisation function
void init_mpu6050() {
    // Configure communication with chip
    Wire.begin();           // Initialize comunication
    Wire.setClock(400000);  // 400kHz I2C clock. Comment this line if having compilation difficulties

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
    Wire.setWireTimeout(3000, true);
#endif

    mpu.setSleepEnabled(false);

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(0);   // 51
    mpu.setYGyroOffset(0);   // 8
    mpu.setZGyroOffset(0);   // 21
    mpu.setXAccelOffset(0);  // 1150
    mpu.setYAccelOffset(0);  //-50
    mpu.setZAccelOffset(0);  // 1060

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        calibrate_mpu();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

// Read temp sensor data (not available from DMP)
void read_MPU_temp_data() {
    static const unsigned int temp_division_factor{340};  // According to the datasheet
    static const float temp_addition_factor{36.53};       // According to the datasheet

    // Read acceleromter data
    Wire.beginTransmission(MPU);
    Wire.write(0x41);  // Start with register 0x41 (TEMP_OUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 2, true);  // Read temp 2 reg
    temperature = (Wire.read() << 8 | Wire.read());
    temperature = (temperature / temp_division_factor) + temp_addition_factor;
}