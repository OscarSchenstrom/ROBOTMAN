#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "stdint.h"

int16_t normalize_range_bodypart(int16_t bodypart, int i);

void switch_stance(int stance);

void body_part_max_min(int limb);

float* setup_parts();

uint16_t* setup_centers();

void move_limb_to(int16_t limb, int16_t location);

void calculations();

void keep_balance(float this_yaw, float this_pitch, float this_roll);

#endif  // MOVEMENT_H