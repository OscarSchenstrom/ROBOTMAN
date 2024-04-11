
#include "../inc/movement.h"

#include "../inc/serial_communication.h"
#include "../inc/variables.h"

// Translates the value of a servo to a normalized value between -100 and 100
int16_t normalize_range_bodypart(int16_t bodypart, int i) {
    float range{joint_definitions[i][JOINT_MAX] - joint_definitions[i][JOINT_MIN]};
    float normalized_pos{bodypart - joint_definitions[i][JOINT_MIN]};
    int16_t return_value{((normalized_pos / range) * 2.0f - 1) * 100 * joint_definitions[i][JOINT_DIR]};
    return return_value;
}

void switch_stance(int stance) {
    for (int i{0}; i < BODYPART_COUNT; i++) {
        move_limb_to(i, normalize_range_bodypart(stance_array[i][stance], i));
        delay(50);  // Annoying delay to keep servos from cramping up
    }
}

void body_part_max_min(int limb) {
    if (joint_current_position[limb].normalized_position >= 0) {
        move_limb_to(limb, -100);
    } else {
        move_limb_to(limb, 100);
    }
}

// Setups 1/200(-100:100) part of each servos movements
float* setup_parts() {
    static float parts[BODYPART_COUNT];
    for (int i = 0; i < BODYPART_COUNT; i++) {
        parts[i] = (joint_definitions[i][JOINT_MAX] - joint_definitions[i][JOINT_MIN]) * 0.01f * 0.5f;
    }
    return parts;
}

uint16_t* setup_centers() {
    static uint16_t centers[BODYPART_COUNT];
    for (int i = 0; i < BODYPART_COUNT; i++) {
        centers[i] = (uint16_t)((joint_definitions[i][JOINT_MAX] + joint_definitions[i][JOINT_MIN]) * 0.5f);
    }
    return centers;
}

void move_limb_to(int16_t limb, int16_t location) {
    static const float* parts{setup_parts()};
    static const uint16_t* centers{setup_centers()};

    float part{parts[limb]};
    uint16_t center{centers[limb]};
    int16_t wanted_location{(int16_t)center + part * location * joint_definitions[limb][JOINT_DIR]};

#if SAFEMODE
    if (wanted_location > (center + part * 100)) {
        wanted_location = center + part * 100;
        location = 100;
    }
    if (wanted_location < (center - part * 100)) {
        wanted_location = center - part * 100;
        location = -100;
    }
#endif

    if (wanted_location <= 0) {
        Serial.print("Location too low: ");
        Serial.println(wanted_location);
        return;
    }

    // BODYPART_COUNT - 1 as long as HEAD is not active
    if (limb >= 0 && limb < BODYPART_COUNT - 1) {
        pwm.setPWM(limb, 0, wanted_location);
    }

    joint_current_position[limb].actual_position = wanted_location;
    joint_current_position[limb].normalized_position = location;
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
        0,                        // HEAD
    };

    for (int i{0}; i < BODYPART_COUNT - 1; i++) {
        int16_t location{
            normalize_range_bodypart(stance_array[i][STANCE_CURRENT] +
                                         displacement[i] * joint_definitions[i][JOINT_DIR],
                                     i)};
        move_limb_to(i, location);
    }
}