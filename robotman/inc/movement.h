#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "stdint.h"
#include "variables.h"

int16_t normalize_servo_value(int16_t value, int bodypart);

void switch_stance(Stance stance);

void body_part_max_min(Bodypart limb);

void move_limb_to(Bodypart limb, int16_t normalized_location);

void calculations();

void keep_balance(float this_yaw, float this_pitch, float this_roll);

#endif  // MOVEMENT_H