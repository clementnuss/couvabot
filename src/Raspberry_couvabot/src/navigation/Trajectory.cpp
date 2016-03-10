//
// Created by clement on 10.03.2016.
//

#include "Trajectory.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include "../physIface/robotConstants.h"

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

    double pL, pR;
    if (leftRot) {
        pL = radius / (radius + AXLE);
        pR = 1f;
    } else {
        pL = 1f;
        pR = radius / (radius + AXLE);
    }
    pL *= speed;
    pR *= speed;
    return gearsPower{pL, pR};
}

double Trajectory::updateAngle() {
    return 0;
}

double Trajectory::computeAngle(double alpha, double d, double rem, double a) {

    double delta = M_PI - asin((d * sin(alpha)) / a);
    double kappa = asin(rem / a);

    return delta - kappa;
}