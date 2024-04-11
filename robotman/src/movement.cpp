
#include "../inc/movement.h"

#include "../inc/serial_communication.h"
#include "../inc/variables.h"

/**
 * @brief Function translates the value of a servo to a normalized value between NORMALIZED_RANGE_MIN (-100) and NORMALIZED_RANGE_MAX (100)
 * @param value is the servo value to be normalized
 * @param bodypart is used to collect the MIN and MAX servo values for that bodypart from joint_definitions
 * 
 * @return A normalized servo value
*/
int16_t normalize_servo_value(int16_t value, int bodypart) {
    
    int16_t normalized_range;
    normalized_range = map(value, joint_definitions[bodypart][JOINT_MIN], joint_definitions[bodypart][JOINT_MAX], NORMALIZED_RANGE_MIN, NORMALIZED_RANGE_MAX);
    return normalized_range;
}

/**
 * @brief Function switches stance by moving all the limbs
 * @param stance is the new stance
 * 
*/
void switch_stance(Stance stance) {
    for (int i{LEFT_SHOULDER}; i < BODYPART_COUNT; i++) {
        move_limb_to(i, normalize_servo_value(stance_array[i][stance], i));
        delay(500);  // Annoying delay to keep servos from cramping up
    }
}

/**
 * @brief Function will move bodypart to the end position opposite to where it currently is.
 * @param limb is the bodypart to be moved
 * 
*/
void body_part_max_min(Bodypart limb) {
    if (joint_current_position[limb].normalized_position >= 0) {
        move_limb_to(limb, NORMALIZED_RANGE_MIN);
    } else {
        move_limb_to(limb, NORMALIZED_RANGE_MAX);
    }
}

/**
 * @brief Function moves limb to a location.
 * @param limb is the part of the body that will be moved.
 * @param normalized_location is the location it will be moved to.
 * 
 * @note normalized_location needs to be within the NORMALIZED_RANGE_MIN and NORMALIZED_RANGE_MAX.
*/
void move_limb_to(Bodypart limb, int16_t normalized_location) {

    int16_t wanted_location;
    wanted_location = map(normalized_location, NORMALIZED_RANGE_MIN, NORMALIZED_RANGE_MAX, joint_definitions[limb][JOINT_MIN], joint_definitions[limb][JOINT_MAX]);

    if (wanted_location < 0 || wanted_location > 4096) {
        Serial.print(F("Out of range (0-4096): "));
        Serial.println(wanted_location);
        return;
    }

    if (limb >= 0 && limb < BODYPART_COUNT) {
        pwm.setPWM(limb, 0, wanted_location);
    }

    joint_current_position[limb].actual_position = wanted_location;
    joint_current_position[limb].normalized_position = normalized_location;
}

void calculations() {
    // Read a packet from FIFO
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {  // Get the Latest packet

        float last_yaw, last_pitch, last_roll, new_yaw, new_pitch, new_roll, some_gain;

        some_gain = 0.1;  // must be positive but under 1
        last_yaw = yaw;
        last_pitch = pitch;
        last_roll = roll;

        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        new_yaw = ypr[2] * 180 / M_PI;     // yaw
        new_roll = -ypr[1] * 180 / M_PI;   // roll (-)
        new_pitch = -ypr[0] * 180 / M_PI;  // pitch (-)

        // Low pass filter to get rid off jitter values
        yaw = some_gain * last_yaw + (1 - some_gain) * new_yaw;
        roll = some_gain * last_roll + (1 - some_gain) * new_roll;
        pitch = some_gain * last_pitch + (1 - some_gain) * new_pitch;
    }
}

// Keep balance function
void keep_balance(float this_yaw, float this_pitch, float this_roll) {
    float gain_scale{2.6};

    int displacement[] = {
        this_pitch * gain_scale,  // LEFT_SHOULDER
        this_pitch * gain_scale,  // RIGHT_SHOULDER
        this_roll * gain_scale,   // LEFT_ARM
        this_roll * gain_scale,   // RIGHT_ARM
        0,                        // LEFT_WRIST
        0,                        // RIGHT_WRIST
        this_roll * gain_scale,   // LEFT_HIP
        this_roll * gain_scale,   // RIGHT_HIP
        this_pitch * gain_scale,  // LEFT_THIGH
        this_pitch * gain_scale,  // RIGHT_THIGH
        this_pitch * gain_scale,  // LEFT_KNEE
        this_pitch * gain_scale,  // RIGHT_KNEE
        this_pitch * gain_scale,  // LEFT_ANKLE
        this_pitch * gain_scale,  // RIGHT_ANKLE
        this_roll * gain_scale,   // LEFT_FOOT
        this_roll * gain_scale,   // RIGHT_FOOT
    };

    for (Bodypart bodypart{LEFT_SHOULDER}; bodypart < BODYPART_COUNT; bodypart++) {
        int16_t location{
            normalize_servo_value(stance_array[bodypart][STANCE_CURRENT] +
                                         displacement[bodypart] * joint_definitions[bodypart][JOINT_DIR],
                                     bodypart)};
        move_limb_to(bodypart, location);
    }
}