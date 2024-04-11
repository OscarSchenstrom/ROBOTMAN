#include "inc/inputs.h"
#include "inc/movement.h"
#include "inc/mpu.h"
#include "inc/serial_communication.h"
#include "inc/ultrasonic.h"
#include "inc/variables.h"

#define INPUT_HANDLER true
#define SAFEMODE true

void setup() {
    // Initialize serial communication
    unsigned int baud_rate{9600};
    Serial.begin(baud_rate);
    delay(100);

    pwm.begin();
    pwm.setOutputMode(false);

    // activate I2C pull up resistors
    digitalWrite(SDA, 1);
    digitalWrite(SCL, 1);

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
    // Iinitialize accelerometer
    init_mpu6050();

    // Initialize ultrasonic sensor
    pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
    pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
#endif

    const byte pwm_freq{50};  // Hz
    pwm.setPWMFreq(pwm_freq);

    // Initialize the pulse that will control the motors
    pwm.setOutputMode(true);
    delay(100);

    // Set robot initial stance from boot
    switch_stance(STANCE_IDLE);

    // If MPU programming failed, don't try to do anything, try again
    if (!dmpReady)
        return;
}

long readVcc() {
    long result;
    // Read 1.1V reference against AVcc
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2);             // Wait for Vref to settle
    ADCSRA |= _BV(ADSC);  // Convert
    while (bit_is_set(ADCSRA, ADSC))
        ;
    result = ADCL;
    result |= ADCH << 8;
#endif
    result = 1126400L / result;  // Back-calculate AVcc in mV
    Serial.println(result);
    return result;
}

void loop() {
    uint16_t currentTime{millis()};

    // readVcc();
#if INPUT_HANDLER
    handle_inputs();
#endif

    calculations();

    if (BALANCE_MODE) {
        keep_balance(yaw, pitch, roll);
    }

    if (DEBUG_MODE) {
        debug(currentTime);
    }
}

void debug(uint16_t currentTime) {
    static uint16_t previousTime{currentTime};
    static uint16_t clockCycle{50};

    int16_t time_diff{currentTime - previousTime};

    if (time_diff > (2 * clockCycle + 20)) {
        print_values();
        previousTime = millis();
    } else if (time_diff > (2 * clockCycle)) {
        // Only read temp data every xth iteration of the main loop
        read_MPU_temp_data();
    } else if (time_diff > clockCycle) {
        // Only read distance data every xth iteration of the main loop
        distanceMeasure();
    }
}