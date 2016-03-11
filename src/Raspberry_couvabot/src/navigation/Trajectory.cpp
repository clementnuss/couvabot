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
    } else {
        leftRot = false;
    }
    this->tx = tx;
    this->ty = ty;

    it = 0;

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

    time = startTime = 0;

    vL = mvmtController::powerToSpeed(pL);
    vR = mvmtController::powerToSpeed(pR);
    return gearsPower{pL, pR};
}

int Trajectory::updateAngle() {


    cout << "micros " << micros() << " and time : " << time << "\n";
    if ((micros() - time) < 1000)
    {
        cout << "nothing to do\n";
        return 0;
    }


    double deltaT = micros() - time; // Delta t since last update
    double deltaTheta = ((vL - vR) / AXLE) * (deltaT * 0.000001);

cout << "Delta T : " << deltaT << "\n";

    theta -= deltaTheta;
    time = micros();

    cout << "Theta modified \n";
    it++;

    if (theta <= 0.00872664) {    // if theta <= 0.5 [deg]
        cout << "End of trajectory at time " << time << ", after " << it << " iterations\n";
        return 1;
    }

    return 0;
}

double Trajectory::computeAngle(double alpha, double d, double rem, double a) {

    double delta = M_PI - asin((d * sin(alpha)) / a);
    double kappa = asin(rem / a);

    return delta - kappa;
}

void Trajectory::start() {
    startTime = micros();
    time = startTime;
}
