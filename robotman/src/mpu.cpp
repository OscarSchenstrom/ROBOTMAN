#include "../inc/mpu.h"

#include "../inc/serial_communication.h"
#include "../inc/variables.h"

#define PRECISE_CALIBRATION 1
#define NBR_OF_CALIBRATED_AXIS 6

//Change this 3 variables if you want to fine tune the skecth to your needs.
uint16_t buffersize=100;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
uint8_t acel_deadzone=1;     //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
uint8_t giro_deadzone=1;     //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)

int16_t ax, ay, az,gx, GY, gz;

int mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz,state=0;
int ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset;

void meansensors() {
    long i=0,buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;
    
    while (i<(buffersize+1)) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &GY, &gz);
        
        buff_ax=buff_ax+ax;
        buff_ay=buff_ay+ay;
        buff_az=buff_az+az;
        buff_gx=buff_gx+gx;
        buff_gy=buff_gy+GY;
        buff_gz=buff_gz+gz;

        if (i==(buffersize)){
            mean_ax=buff_ax/buffersize;
            mean_ay=buff_ay/buffersize;
            mean_az=buff_az/buffersize;
            mean_gx=buff_gx/buffersize;
            mean_gy=buff_gy/buffersize;
            mean_gz=buff_gz/buffersize;
        }
        i++;
    }
}

void calibration() {
    
    ax_offset=-mean_ax/8;
    ay_offset=-mean_ay/8;
    az_offset=(16384-mean_az)/8;

    gx_offset=-mean_gx/4;
    gy_offset=-mean_gy/4;
    gz_offset=-mean_gz/4;

    while (1) {
        int ready=0;
        mpu.setXAccelOffset(ax_offset);
        mpu.setYAccelOffset(ay_offset);
        mpu.setZAccelOffset(az_offset);

        mpu.setXGyroOffset(gx_offset);
        mpu.setYGyroOffset(gy_offset);
        mpu.setZGyroOffset(gz_offset);

        meansensors();

        if (abs(mean_ax)<=acel_deadzone) ready++;
        else ax_offset=ax_offset-mean_ax/acel_deadzone;

        if (abs(mean_ay)<=acel_deadzone) ready++;
        else ay_offset=ay_offset-mean_ay/acel_deadzone;

        if (abs(16384-mean_az)<=acel_deadzone) ready++;
        else az_offset=az_offset+(16384-mean_az)/acel_deadzone;

        if (abs(mean_gx)<=giro_deadzone) ready++;
        else gx_offset=gx_offset-mean_gx/(giro_deadzone+1);

        if (abs(mean_gy)<=giro_deadzone) ready++;
        else gy_offset=gy_offset-mean_gy/(giro_deadzone+1);

        if (abs(mean_gz)<=giro_deadzone) ready++;
        else gz_offset=gz_offset-mean_gz/(giro_deadzone+1);

        Serial.print(ready);
        Serial.print(F(" of "));
        Serial.print(NBR_OF_CALIBRATED_AXIS);
        Serial.println(F(" axis calibrated"));
        if (ready==NBR_OF_CALIBRATED_AXIS){
            Serial.println(F("Calibration Successful"));
            break;
        }
    }
}

void dmpDataReady() {
    mpuInterrupt = true;
}     

bool calibrate_mpu() {
    if (devStatus != 0) {
        Serial.println(F("MPU not initialized!"));
        return false;
    }
    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.setDMPEnabled(false);

#if PRECISE_CALIBRATION
    meansensors();

    calibration();

    meansensors();

    mpu.setXAccelOffset(ax_offset);
    mpu.setYAccelOffset(ay_offset);
    mpu.setZAccelOffset(az_offset);
    mpu.setXGyroOffset(gx_offset);
    mpu.setZGyroOffset(gz_offset); 

#else
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
#endif   

    Serial.println();
    mpu.PrintActiveOffsets();
    // turn on the DMP, now that it's ready
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready!"));
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

    mpu.initialize((ACCEL_FS)MPU6050_ACCEL_FS_16, (GYRO_FS)MPU6050_GYRO_FS_2000);
    pinMode(INTERRUPT_PIN, INPUT);

    Serial.println(mpu.testConnection() ? F("MPU6050 - OK!") : F("MPU6050 - Connection Failed"));

    Serial.println(F("Init DMP..."));
    devStatus = mpu.dmpInitialize();
    mpu.setFullScaleAccelRange(3);

    // Resets offsets before we calibrate
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);

    if (devStatus == 0) {
        calibrate_mpu();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        Serial.print(F("DMP Init failed (code "));
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