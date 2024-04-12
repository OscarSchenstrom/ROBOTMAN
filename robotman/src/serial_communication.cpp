#include "../inc/serial_communication.h"

#include "../inc/variables.h"

// Print the values
void print_values() {
    // Print the MPU sensor values on the serial bus
    Serial.print(temperature);  // temperature
    Serial.print(F("/"));

    Serial.print(yaw);  // yaw
    Serial.print(F("/"));
    Serial.print(roll);  // roll
    Serial.print(F("/"));
    Serial.print(pitch);  // pitch
    Serial.print(F("/"));

    // Prints the ultrasonic measured distance on the Serial Monitor
    Serial.print(ultra_sonic_distance);

    for (int i = 0; i < BODYPART_COUNT; i++) {
        Serial.print(F("/"));
        Serial.print(joint_current_position[i].normalized_position);
    }

    Serial.println();
}