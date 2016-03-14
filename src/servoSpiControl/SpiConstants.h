#ifndef SERVOSPICONTROL_SPICONSTANTS_H
#define SERVOSPICONTROL_SPICONSTANTS_H

// DRIVING GEARS
#define FORWARD         0
#define BACKWARD        1
#define BOTH            2
#define MOTOR_A         0
#define MOTOR_B         1
#define STOP            3
#define MAX_SPEED       130
#define CW              0
#define CCW             1
#define ROT_L           4
#define ROT_R           5

// SERVO MANEUVERS
#define B_CLOSE         90
#define B_OPEN          10
#define B_LAPSE         10
#define B_ROLL_LAPSE    1000
#define B_ROLL_SPEED    110
#define B_RELEASE       3
#define B_ROLL          4
#define B_ROLLING       5
#define B_RECLOSE       6
#define F_LAPSE         10
#define F_CATCH_G       3
#define F_CATCH_R       4
#define F_IR_G          5
#define F_IR_R          6
#define F_BELT          7
#define F_PULL          8
#define F_PULL_LAPSE    4900
#define F_LIFT          9
#define F_LIFT_LAPSE    8000
#define M_MID           90
#define M_RED           30
#define M_GREEN         150
#define L_OPEN          180
#define L_CLOSE         100
#define L_PUSH          90
#define L_SAFE          150
#define R_OPEN          1
#define R_CLOSE         80
#define R_PUSH          90
#define R_SAFE          30

// SENSORS IO
#define IR_CATCH_G      0
#define IR_CATCH_R      1
#define IR_DIST         512
#define IR_CATCH_LAPSE  1000

//-------------------PINS TO CHANGE----------------------
#define STBY_GEAR       26 //standby, can be digital pin

// Motor A
#define PWMA_GEAR       9 //Speed control, PWM pin
#define AIN1_GEAR       22 //Direction, can be digital pin
#define AIN2_GEAR       23 //Direction

// Motor B
#define PWMB_GEAR       8
#define BIN1_GEAR       24
#define BIN2_GEAR       25

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

// Buttons and IRs
#define CATCH_BUTTON    40 // Button digital pin
#define IR_ENABLE       30 // IR digital pin
#define IR_CATCH_G_PIN      A1 // IR analog pin
#define IR_CATCH_R_PIN      A2


#endif //SERVOSPICONTROL_SPICONSTANTS_H
