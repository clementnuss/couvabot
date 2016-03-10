//
// Created by clement on 10.03.2016.
//

#define _USE_MATH_DEFINES

#include <math.h>

#include "Trajectory.h"

using mvmtCtrl::mvmtController;

#include "../physIface/robotConstants.h"
#include "../physIface/localTime.h"

//TODO: manage angles greater than -pi/2 , pi/2
Trajectory::Trajectory(double radius, double tx, double ty) {
    this->radius = radius;
    if (tx < 0) {
        leftRot = true;
        tx = -tx;
    }
    this->tx = tx;
    this->ty = ty;

    double alpha = M_PI_2 - atan2(tx, ty);
    double d = sqrt(pow(tx, 2) + pow(ty, 2));


    double a_2 = pow(d, 2) + pow(radius, 2) - (2 * d * radius * cos(alpha));
    rem = sqrt(a_2 - pow(radius, 2));

    theta = computeAngle(alpha, d, rem, sqrt(a_2));

}


gearsPower Trajectory::getWheelsPower(double speed) {

    if (!startTime)
        start();

    double pL, pR;
    if (leftRot) {
        pL = radius / (radius + AXLE);
        pR = 1;
    } else {
        pL = 1;
        pR = radius / (radius + AXLE);
    }
    pL *= speed;
    pR *= speed;

    vL = mvmtController::powerToSpeed(pL);
    vR = mvmtController::powerToSpeed(pR);
    return gearsPower{pL, pR};
}

int Trajectory::updateAngle() {

    if ((millis() - time) <= 0)
        return 0;

    double deltaT = millis() - time; // Delta t since last update
    double deltaTheta = ((vL - vR) / AXLE) * deltaT;
    theta -= deltaTheta;

    if (theta <= 0.00872664)    // if theta <= 0.5 [deg]
        return 1;
    else
        return 0;
}

double Trajectory::computeAngle(double alpha, double d, double rem, double a) {

    double delta = M_PI - asin((d * sin(alpha)) / a);
    double kappa = asin(rem / a);

    return delta - kappa;
}

void Trajectory::start() {
    startTime = millis();
}
