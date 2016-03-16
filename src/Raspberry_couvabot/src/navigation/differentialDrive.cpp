//
// Created by clement on 15.03.2016.
//

#include "differentialDrive.h"
#include "../robotConstants.h"

int const middleX = FRAME_WIDTH / 2;
double const diffCoeff = 0.15;

gearsPower getParams(int x, int y, double speed) {

    bool left = false;

    int centeredX = x - middleX;
    if (centeredX < 0) {
        left = true;
        centeredX = -centeredX;
    }

    double normalizedDiff = (double) centeredX / middleX;
    double k = normalizedDiff * diffCoeff;

    //Adjust if the speed is too high
    if (speed + k > 1)
        speed -= (speed + k) - 1;

    if (left)
        return {speed - k, speed + k};
    else
        return {speed + k, speed - k};
}
