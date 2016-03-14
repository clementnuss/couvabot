//
// Created by clement on 07.03.2016.
//

#ifndef ARDUINO_COUVABOT_COUVABOT_H
#define ARDUINO_COUVABOT_COUVABOT_H

#include <Arduino.h>
#include <avr/io.h>

#include "src/Motor.h"

#define FORWARD     0
#define BACKWARD    1
#define BOTH        2
#define MOTOR_A     0
#define MOTOR_B     1
#define STOP        3
#define MAX_SPEED   180
#define CW          0
#define CCW         1
#define ROT_L       4
#define ROT_R       5

//-------------------PINS TO CHANGE----------------------
#define STBY_GEAR  27 //standby

//Motor A
#define PWMA_GEAR  3 //Speed control
#define AIN1_GEAR  22 //Direction
#define AIN2_GEAR  23 //Direction

//Motor B
#define PWMB_GEAR  2 //Speed control
#define BIN1_GEAR  24 //Direction
#define BIN2_GEAR  25 //Direction
//-------------------------------------------------------

typedef struct IR_SENSOR IR_SENSOR;
struct IR_SENSOR{
    char pin1;
    char pin2;
    unsigned int value;
};



// Function declaration ----------------------------------
int angleConvert(int rotAngle, int rotSpeed);

void commandDecoder();

void motorDecoder();

void spiHandler();

void initSPI();

void sendData();


#endif //ARDUINO_COUVABOT_COUVABOT_H
