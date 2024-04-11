#ifndef MPU_H
#define MPU_H

void dmpDataReady();

bool calibrate_mpu();

void init_mpu6050();

void read_MPU_temp_data();

#endif  // MPU_H