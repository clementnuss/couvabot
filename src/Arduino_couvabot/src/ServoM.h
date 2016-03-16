//
// Created by clement on 16.03.2016.
//

#ifndef ARDUINO_COUVABOT_SERVOM_H
#define ARDUINO_COUVABOT_SERVOM_H

#define ARDUINO_ARCH_AVR
#include "../../Servo/Servo.h"

class ServoM {
    // Class Member Variables
    // These are initialized at startup
    Servo servo;
    int resetPos;                      // Servo position at startup

public:
    volatile int anglePos;               // Position of servo at any time

    ServoM(int rPos);

    void Attach(int pin);

    void Detach();

    void reset(void);

    void writePos(int angle);

    void updatePos(void);
};


#endif //ARDUINO_COUVABOT_SERVOM_H
