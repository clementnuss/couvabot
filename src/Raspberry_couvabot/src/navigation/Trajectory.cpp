//
// Created by clement on 10.03.2016.
//

#define _USE_MATH_DEFINES

#include <math.h>

#include "Trajectory.h"

using mvmtCtrl::mvmtController;

#include "../physIface/robotConstants.h"
#include "../physIface/localTime.h"

/*
 * Default constructor, that initializes default variables
 */

Trajectory::Trajectory() {
    radius = -1;
    tx = 0;
    ty = 0;
    it = 0;
}

/*
 *  Function that computes the wheels power, given a speedPer in {0,1}
 */
gearsPower Trajectory::setWheelsPower(double spd) {

    if (straight) {
        this->speedPer = spd;
        return gearsPower{spd, spd};
    }

    if (!time)
        start();

    double pL, pR;
    if (leftRot) {
        pL = differentialRatio;
        pR = 1;
    } else {
        pL = 1;
        pR = differentialRatio;
    }
    pL *= spd;
    pR *= spd;
    speedPer = spd;

    //TODO: insert calibration value!
    vL = mvmtController::powerToSpeed(pL);
    vR = mvmtController::powerToSpeed(pR);
    speedRatio = (vL - vR) / AXLE;

    return gearsPower{pL, pR};
}


gearsPower Trajectory::getWheelsPower() {
    if (straight)
        return {speedPer, speedPer};

    double pL, pR;
    if (leftRot) {
        pL = differentialRatio;
        pR = 1;
    } else {
        pL = 1;
        pR = differentialRatio;
    }
    pL *= speedPer;
    pR *= speedPer;

    return {pL, pR};
}

/*
 *  Method used to check the parameters of the trajectory.
 *  It returns 0 if there is nothing to change, otherwise
 *  it returns 1.
 */

int Trajectory::update() {

    if (straight) {
        if (trajectoryEnded)
            return 1;

        if ((micros() - time) < 1000)   // check distance every ms
            return 0;

        double deltaT = (micros() - time) * 0.000001; // Delta t since last update [us]
        double deltaD = deltaT * mvmtController::powerToSpeed(speedPer);

        rem -= deltaD;
        time = micros();
        it++;

        if (rem <= 0.05) {
            trajectoryEnded = true;
            cout << "End of trajectory at time " << millis() << ", after " << it << " iterations\n";
            return 1;
        }
    }

    if ((micros() - time) < 1000)   // check angle every ms
        return 0;

    double deltaT = (micros() - time) * 0.000001; // Delta t since last update [us]
    double deltaTheta = (speedRatio * deltaT);

    theta -= deltaTheta;
    time = micros();
    it++;

    if (theta <= 0.00872664) {    // if theta <= 0.5 [deg]
        cout << "End of rotation at time " << millis() << ", after " << it << " iterations\n";
        straight = true;
        it = 0;
        return 1;
    }

    return 0;
}

/*
 * Utilitary function that computes the rotation angle
 */

double Trajectory::computeAngle(double alpha, double d, double rem, double a) {

    double delta = M_PI - asin((d * sin(alpha)) / a);
    double kappa = asin(rem / a);

    return delta - kappa;
}

/*
 * Call this function before using update function, or you'll get
 * screwed results !
 */

void Trajectory::start() {
    time = micros();
}

/*
 * Function setting parameters of the trajectory
 */

void Trajectory::setParams(double radius, double tx, double ty) {
    this->radius = radius;
    if (tx < 0) {
        leftRot = true;
        tx = -tx;
    } else {
        leftRot = false;
    }
    this->tx = tx;
    this->ty = ty;
    it = 0;

    trajectoryEnded = false;

    double alpha = M_PI_2 - atan2(tx, ty);
    double d = sqrt(pow(tx, 2) + pow(ty, 2));
    double a_2 = pow(d, 2) + pow(radius, 2) - (2 * d * radius * cos(alpha));
    rem = sqrt(a_2 - pow(radius, 2));
    theta = computeAngle(alpha, d, rem, sqrt(a_2));
    differentialRatio = radius / (radius + AXLE);

    if (theta <= 0.00872664) {    // if theta <= 0.5 [deg]
        straight = true;
    }
}
