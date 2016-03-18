//
// Created by clement on 12.03.2016.
//

#ifndef COUVABOT_HEARTBEAT_H
#define COUVABOT_HEARTBEAT_H


#include "SPICom.h"


class HeartBeat {

private:
    SPICom *spiCom;
    uint8_t readData;
    unsigned int time;

public:

    arduinoData getData();

    HeartBeat(SPICom *spiCom);

    int pingArduino();

    int start();
};


#endif //COUVABOT_HEARTBEAT_H
