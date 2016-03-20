/*
 * Braitenberg basis
 * We have many ways to implement this, we can read IR sensors all the time and
 * adapt speeds accordingly if an obstacle arises - or
 * we can read IR sensors and if sum(sensors) > threshold_A, enter braitenberg until
 * sum(sensors) < threshold_B; with threshold_B < threshold_A.
 */

#include <math.h>
#include "braitenberg.h"

#define NB_IR_SENSORS 8        // Number of IR sensors

// Usually, IR sensors range from [0; X], X being the closest position for an obstacle
// whereas 0 means that nothing is seen.

// Exemplary function: run braiten providing the variable for left and right wheel_speed
// speeds, these will be updated according to IR sensors.
void braiten(gearsPower *powers, ardCom::sensorsData sens) {
    // Braitenberg coefficients for left and right wheels if MAX_SPEED == 1
    static double l_weight[NB_IR_SENSORS] = {-1.5, -0.6, 0.3, 0, 0, 0, 0, 0};
    static double r_weight[NB_IR_SENSORS] = {0, 0, 0, 0, 0, 0.3, -0.6, -1.5};

/*
    static double l_weight[NB_IR_SENSORS] = {-0.375, -0.25, -0.125, 0, 0.03125, 0.0625, 0.09375, 0.3125};
    static double r_weight[NB_IR_SENSORS] = {0.3125, 0.09375, 0.05625, 0.025, 0, -0.1, -0.25, -0.375};
*/

    // Distance sensors

    for (int j = 0; j < 8; ++j) {
        printf("%ud \t",sens.values[j]);
    }
    cout << endl;

    // Get distance sensors
    int i;

    // Bias speeds (when sensors do not detect anything)
    // Two options - pick one
    /*  1. use FORWARD_SPEED as the bias speed
    double left_speed  = FORWARD_SPEED;
    double right_speed = FORWARD_SPEED;
    //*/
    //* 2. use given speeds to add braitenberg on top of it
    double left_speed = powers->pL;
    double right_speed = powers->pR;
    //*

    // Modify speed according to sensory feedback
    for (i = 0; i < NB_IR_SENSORS; i++) {
        left_speed += l_weight[i] * ((1.0 - sens.values[i] / 255.));
        right_speed += r_weight[i] * ((1.0 - sens.values[i] / 255.));
    }

    // Bound speeds
    double reduction_factor;
    if (fabs(right_speed) > 1. || fabs(left_speed) > 1.) {
        // Determine biggest reduction factor
        if (fabs(right_speed) > fabs(left_speed))
            reduction_factor = right_speed;
        else
            reduction_factor = left_speed;

        // Divide speeds by that factor so that highest speed is now equal to MAX_SPEED
        left_speed /= reduction_factor;
        right_speed /= reduction_factor;
    }

    cout << "left speed: " << left_speed << " and right speed: " << right_speed << "\n";

    // Update wheel speeds
    powers->pL = left_speed;
    powers->pR = right_speed;
}