//
// Created by clement on 12.03.2016.
//

#ifndef COUVABOT_HEARTBEAT_H
#define COUVABOT_HEARTBEAT_H


#include "SPICom.h"

class heartBeat {

private:
    SPICom *spiCom;
    uint8_t readData;

public:

    getData();

    heartBeat(SPICom *spiCom);

    int pingArduino();

};


#endif //COUVABOT_HEARTBEAT_H
