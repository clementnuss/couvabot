//
// Created by clement on 04.03.2016.
//

#ifndef COUVABOT_MOVCONTROLLER_H
#define COUVABOT_MOVCONTROLLER_H

#include "SPICom.h"

namespace ardCom {

    struct gearsPower {
        double pL;  // left gear percentage
        double pR;  // right gear percentage
    };

    struct sensorsData {
        uint8_t values[8];
        bool valid;
    };


    class arduinoComm {
        int maxPWM, pollTime;
        SPICom *spiCom;
        uint8_t readData;

    public:

        arduinoComm(SPICom *spiCom, double vBat);

        bool gearsCommand(gearsPower powers);

        static double powerToSpeed(double power);

        bool catchPuck();

        bool prepareLeft();

        bool prepareRight();

        bool releasePuck();

        sensorsData getData();

        int start();

    private:

        uint8_t getPWM(double p);

    };
}
#endif //COUVABOT_MOVCONTROLLER_H
