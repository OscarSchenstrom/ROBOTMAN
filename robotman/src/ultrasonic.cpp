#include "../inc/ultrasonic.h"

#include "../inc/variables.h"

// All the functions regarding the ultrasonic sensor

// Calculate distance. Returns distance in cm
unsigned int ultraSonic() {
    static const float speed_of_sound{0.034};  // (In air) cm/us

    // Clear trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Set the trigPin on HIGH for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    unsigned long duration_us = pulseIn(echoPin, HIGH, 10000);
    // Calculating the distance
    // Divide by two since the object is half the distance the sound travels
    return duration_us * speed_of_sound / 2;
}

// Beep piezo element when object is too close
void distanceMeasure() {
    // Piezo element global variables
    static byte piezo_pin{11};
    static byte beep_tone{50};
    static byte duration{50};

    // Get distance
    ultra_sonic_distance = ultraSonic();

    if (ultra_sonic_distance > 40) {
        return;
    } else if (ultra_sonic_distance > 20) {
        beep_tone = 110;
    } else if (ultra_sonic_distance > 10) {
        beep_tone = 130;
    } else if (ultra_sonic_distance > 5) {
        beep_tone = 150;
    } else if (ultra_sonic_distance > 0) {
        beep_tone = 170;
    } else {
        ultra_sonic_distance = -1;
    }

    tone(piezo_pin, beep_tone, duration);
}