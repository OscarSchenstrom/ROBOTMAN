#include "../inc/serial_communication.h"

#include "../inc/variables.h"

// Print the values
void print_values() {
    // Print the MPU sensor values on the serial bus
    Serial.print(temperature);  // temperature
    Serial.print("/");

    Serial.print(yaw);  // yaw
    Serial.print("/");
    Serial.print(roll);  // roll
    Serial.print("/");
    Serial.print(pitch);  // pitch
    Serial.print("/");

    // Prints the ultrasonic measured distance on the Serial Monitor
    Serial.print(ultra_sonic_distance);

    for (int i = 0; i < BODYPART_COUNT - 1; i++) {
        Serial.print("/");
        Serial.print(joint_current_position[i].normalized_position);
    }

    Serial.println();
}