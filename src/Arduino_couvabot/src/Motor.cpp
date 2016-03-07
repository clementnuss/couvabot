//
// Created by clement on 07.03.2016.
//

#include "Motor.h"

Motor::Motor(uint8_t pin1, uint8_t pin2, uint8_t pwm) {
    motorPin1 = pin1;
    motorPin2 = pin2;
    motorPWMPin = pwm;
    velocity = 0;
    orient1 = LOW;
    orient2 = LOW;

    pinMode(motorPin1, OUTPUT);   // Can be digital output
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPWMPin, OUTPUT); // Must be analog output
}

void Motor::drive(uint8_t vel, uint8_t orientation) {
    velocity = vel;
    orient1 = LOW;
    orient2 = HIGH;

    if (orientation == BACKWARD) {
        orient1 = HIGH;
        orient2 = LOW;
    }

    digitalWrite(motorPin1, orient1);
    digitalWrite(motorPin2, orient2);
    analogWrite(motorPWMPin, velocity);
}

void Motor::mStop(void) {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    analogWrite(motorPWMPin, 0);

    velocity = 0;
    orient1 = LOW;
    orient2 = LOW;
}