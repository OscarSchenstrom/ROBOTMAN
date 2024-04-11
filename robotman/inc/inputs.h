#ifndef INPUTS_H
#define INPUTS_H

#include "stdint.h"

void handle_inputs();

void print_body_parts();

bool move_limbs_handler(char* serial_input_string);

bool read_data(char* string);

int16_t string_to_int(char* string);

#endif  // INPUTS_H