//
// Created by clement on 07.03.2016.
//

#ifndef ARDUINO_COUVABOT_MOTOR_H
#define ARDUINO_COUVABOT_MOTOR_H

#include "../couvabot.h"

class Motor {
    // Class Member Variables
    // These are initialized at startup
    uint8_t motorPin1;
    uint8_t motorPin2;
    uint8_t motorPWMPin;

    volatile int velocity;
    volatile uint8_t orient1;
    volatile uint8_t orient2;

public:
    Motor(uint8_t pin1, uint8_t pin2, uint8_t pwm);

    void drive(uint8_t vel, uint8_t orientation);

    void mStop(void);
};


#endif //ARDUINO_COUVABOT_MOTOR_H
