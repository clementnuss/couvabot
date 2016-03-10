//
// Created by clement on 04.03.2016.
//

#ifndef COUVABOT_MOVCONTROLLER_H
#define COUVABOT_MOVCONTROLLER_H

#include "SPICom.h"

namespace mvmtCtrl {

    struct gearsPower{
        double pL;  // left gear percentage
        double pR;  // right gear percentage
    };


    class mvmtController {
        int     maxPWM;
        SPICom  *spiCom;
        uint8_t readData, sendData;

    public:

        mvmtController();

        bool move(int x, int y);

        bool arduiCommand(gearsPower powers);

        static double powerToSpeed(double power);

    private:

        uint8_t getPWM(double p);

    };
}
#endif //COUVABOT_MOVCONTROLLER_H
