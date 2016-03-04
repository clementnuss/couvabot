//
// Created by clement on 04.03.2016.
//

#ifndef COUVABOT_MOVCONTROLLER_H
#define COUVABOT_MOVCONTROLLER_H

#include "SPICom.h"

#define VBAT 12

namespace mvmtCtrl {

    class mvmtController {
        int    maxPWM;
        SPICom *arduiCom;
        uint8_t readData, sendData;

    public:

        mvmtController();

        bool move(int x, int y);
        bool arduiCommand(double pL, double pR);

    private:

        uint8_t getPWM(double p);

    };
}
#endif //COUVABOT_MOVCONTROLLER_H
