//
// Created by clement on 10.03.2016.
//

#ifndef COUVABOT_TRAJECTORY_H
#define COUVABOT_TRAJECTORY_H

#include "../physIface/arduinoComm.h"

typedef ardCom::gearsPower gearsPower;

class Trajectory {
private:
    double radius;  // radius of rotation
    double tx, ty;  // target x/y position
    double theta;   // angle of rotation
    double rem;     // remaining distance at end of rotation
    double speedPer, differentialRatio, speedRatio;
    bool leftRot;   // when make a turn to the left, we need to swap the coordinates
    bool straight, trajectoryEnded;  // true when the rotation is ended
    double vR, vL;
    unsigned int time, it;

    double computeAngle(double alpha, double d, double rem, double a);  // Initial theta calculation

public:

    Trajectory();

    gearsPower setWheelsPower(double spd);

    gearsPower getWheelsPower();

    int update();

    void start();

    void setParams(double radius, double tx, double ty);

};

#endif //COUVABOT_TRAJECTORY_H
