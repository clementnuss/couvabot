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
#define STBY_GEAR  24 //standby

//Motor A
#define PWMA_GEAR  10 //Speed control
#define AIN1_GEAR  25 //Direction
#define AIN2_GEAR  26 //Direction

//Motor B
#define PWMB_GEAR  9 //Speed control
#define BIN1_GEAR  27 //Direction
#define BIN2_GEAR  28 //Direction
//-------------------------------------------------------

// Function declaration ----------------------------------
int angleConvert(int rotAngle, int rotSpeed);

void commandDecoder(void);

void motorDecoder(void);

void spiHandler(void);


#endif //ARDUINO_COUVABOT_COUVABOT_H
