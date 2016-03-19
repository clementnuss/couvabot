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
#define MAX_SPEED   150
#define CW          0
#define CCW         1
#define ROT_L       4
#define ROT_R       5

// SERVO MANEUVERS
#define B_L_CLOSE       90
#define B_R_CLOSE       90
#define B_L_OPEN        150
#define B_R_OPEN        45
#define B_LAPSE         30
#define B_ROLL_LAPSE    1000
#define B_FREE_SPEED    40
#define B_RELEASE       3
#define B_ROLL          4
#define B_ROLLING       5
#define B_RECLOSE       6

#define F_LAPSE         15
#define F_CATCH_L       3
#define F_CATCH_R       4
#define F_BELT          7
#define F_PULL          8
#define F_PULL_LAPSE    4100
#define F_ROLL_SPEED    90
#define F_LIFT          9
#define F_LIFT_LAPSE    8000

#define M_WAIT          30
#define M_MID_R         50
#define M_MID_L         90
#define M_MID           90
#define M_RIGHT         125
#define M_LEFT          30

#define L_WAIT          100
#define L_OPEN          179
#define L_CLOSE         90
#define L_PUSH          94
#define L_SAFE          160

#define R_WAIT          80
#define R_OPEN          1
#define R_CLOSE         87
#define R_PUSH          90
#define R_SAFE          17

// SENSORS
#define MAX_IR_SENSOR_VALUE 1023.

//-------------------PINS TO CHANGE----------------------
#define STBY_GEAR  26 //standby

//Motor A
#define PWMA_GEAR  9 //Speed control
#define AIN1_GEAR  22 //Direction
#define AIN2_GEAR  23 //Direction

//Motor B
#define PWMB_GEAR  8 //Speed control
#define BIN1_GEAR  24 //Direction
#define BIN2_GEAR  25 //Direction

// Conveyor belt motor
#define STBY_BELT       27
#define PWM_BELT        7
#define AIN1_BELT       28
#define AIN2_BELT       29

// Servos
#define SERVO_FRONT_L   5  // Servo PWM pins
#define SERVO_FRONT_M   3
#define SERVO_FRONT_R   2
#define SERVO_BACK_R    4
#define SERVO_BACK_L    6

// IR sensors
#define IR_ENABLE       38

// Function declaration ----------------------------------
int angleConvert(int rotAngle, int rotSpeed);

void commandDecoder();

void motorDecoder();

void spiHandler();

void initSPI();

void sendData();

void catchPuck(void);

void catchMoveL(void);

void catchMoveR(void);

void catchMoveR(void);

void freePucks(void);

void servoPrepare(int angle);

void get_ir_sensors();

#endif //ARDUINO_COUVABOT_COUVABOT_H
