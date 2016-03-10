//
// Created by clement on 10.03.2016.
//

#ifndef COUVABOT_TRAJECTORY_H
#define COUVABOT_TRAJECTORY_H

#include "../physIface/mvmtController.h"
typedef mvmtCtrl::gearsPower gearsPower;

class Trajectory {
private:
    double radius;  // radius of rotation
    double tx, ty;  // target x/y position
    double theta;   // angle of rotation
    double rem;       // remaining distance at end of rotation
    bool leftRot;
    double speed;
    int lastTime;

    double computeAngle(double alpha, double d, double rem, double a);  // Initial theta calculation

public:

    Trajectory(double radius, double tx, double ty);
    gearsPower getWheelsPower(double speed);
    double updateAngle();

};

#endif //COUVABOT_TRAJECTORY_H
