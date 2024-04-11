#include "../inc/inputs.h"

#include "../inc/movement.h"
#include "../inc/mpu.h"
#include "../inc/serial_communication.h"
#include "../inc/variables.h"

void handle_inputs() {
    static bool move_limbs_mode{false};
    static bool output_mode_state{true};
    static char input_string[SMALL_STRING_LENGTH];

    if(!MENY_LIST){
        Serial.println();
        Serial.println(F("Command Meny"));
        Serial.println(F("i - Initial Stance"));
        Serial.println(F("m - Move Limbs Meny"));
        Serial.println(F("B - Balance Mode Toggle"));
        Serial.println(F("C - PWM Output Mode Toggle"));
        Serial.println(F("D - Debug Toggle"));
        Serial.println(F("S - List Servo Positions"));
        Serial.println(F("s - Switch stance"));
        Serial.println(F("c - to return to this meny"));
        MENY_LIST = true;
    }

    if (read_data(input_string)) {
        int16_t input_int{string_to_int(input_string)};

        if (move_limbs_mode) {
            if (move_limbs_handler(input_string)) {
                move_limbs_mode = false;
            }
        } else if (input_string[0] == 'c') {
            MENY_LIST = false;
        } else if (input_string[0] == 'i') {
            Serial.println(F("Init stance"));
            STANCE_CURRENT = STANCE_IDLE;
            switch_stance(STANCE_CURRENT);
            MENY_LIST = false;
        } else if (input_string[0] == 'm') {
            print_body_parts();
            move_limbs_mode = true;
        } else if (input_string[0] == 'B') {
            if (BALANCE_MODE) {
                BALANCE_MODE = false;
                Serial.println(F("Balance OFF"));
            } else {
                pwm.setOutputMode(false);
                if (calibrate_mpu()) {
                    output_mode_state = true;
                    BALANCE_MODE = true;
                    Serial.println(F("Balance ON"));
                }
                pwm.setOutputMode(true);
            }
            MENY_LIST = false;
        } else if (input_string[0] == 'C') {
            if (output_mode_state) {
                Serial.println(F("Input off"));
                output_mode_state = false;
            } else {
                Serial.println(F("Input on"));
                output_mode_state = true;
            }
            pwm.setOutputMode(output_mode_state);
            MENY_LIST = false;
        } else if (input_string[0] == 'D') {
            if (DEBUG_MODE) {
                DEBUG_MODE = false;
                Serial.println(F("DEBUG_MODE off"));
                MENY_LIST = false;
            } else {
                DEBUG_MODE = true;
                Serial.println(F("DEBUG_MODE on"));
            }
        } else if (input_string[0] == 'S') {
            for (int i{0}; i < BODYPART_COUNT; i++) {
                Serial.print(i);
                Serial.print(F("\t: "));
                Serial.print(joint_current_position[i].actual_position);
                Serial.print(F(" : "));
                Serial.print(joint_current_position[i].normalized_position);
                Serial.print(F("\t: "));
                Serial.println(Bodypart_name[i]);
            }
            Serial.println(F("c to return"));
        } else if (input_string[0] == 's') {
            if ((++STANCE_CURRENT) >= STANCE_COUNT) {
                STANCE_CURRENT = STANCE_IDLE;
            }
            switch_stance(STANCE_CURRENT);
            MENY_LIST = false;
        } else if (input_int >= 0 && input_int <= 16) {
            body_part_max_min(input_int);
            MENY_LIST = false;
        }
    }
}

void print_body_parts() {
    Serial.println(F("MOVE LIMBS"));
    for (int i = 0; i < BODYPART_COUNT; i++) {
        Serial.print(i);
        Serial.print(F(" : "));
        Serial.print(Bodypart_name[i]);
        Serial.print(F("\t"));
        if ((i % 2) == 1) {
            Serial.println();
        }
    }
    Serial.println();
    Serial.println(F("c to return"));
}

bool move_limbs_handler(char* serial_input_string) {
    static int16_t limb{-1};
    if (serial_input_string[0] == 'c') {
        if (limb == -1) {
            Serial.println(F("exiting"));
            MENY_LIST = false;
            return true;
        } else {
            limb = -1;
            print_body_parts();
            return false;
        }
    }

    int16_t input_int{string_to_int(serial_input_string)};
    if (input_int == INT16_ERROR_VALUE) {
        Serial.println(F("Not a valid value"));
    } else if (limb == -1) {
        limb = input_int;
        if (limb >= BODYPART_COUNT) {
            Serial.println(F("Incorrect limb"));
            limb = -1;
        } else {
            Serial.print(Bodypart_name[limb]);
            Serial.println(F(" chosen. Choose between -100 and 100"));
            Serial.print(F("Current position: "));
            Serial.println(joint_current_position[limb].normalized_position);
            Serial.println(F("c to return"));
        }
    } else {
        move_limb_to(limb, input_int);
        Serial.print(Bodypart_name[limb]);
        Serial.print(F(" moved to "));
        Serial.print(joint_current_position[limb].actual_position);
        Serial.print(F(" : "));
        Serial.println(joint_current_position[limb].normalized_position);

        Serial.println(F("c to return"));
    }
    return false;
}

bool read_data(char* string) {
    static int i = 0;
    while (Serial.available() > 0) {
        char rc{Serial.read()};
        if (rc != '\n') {
            string[i++] = rc;
            if (i >= SMALL_STRING_LENGTH) {
                i = SMALL_STRING_LENGTH - 1;
            }
        } else {
            string[i] = '\0';
            i = 0;
            return true;
        }
    }
    return false;
}

int16_t string_to_int(char* string) {
    if ((string[0] >= '0' && string[0] <= '9') ||
        (string[0] == '-' && (string[1] >= '0' && string[1] <= '9'))) {
        return atoi(string);
    }
    // return an error value
    return INT16_ERROR_VALUE;
}