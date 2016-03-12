//
// Created by clement on 12.03.2016.
//

#include <unistd.h>
#include "heartBeat.h"

int heartBeat::pingArduino() {

    readData = spiCom->CS0_transfer('H');
    switch (readData) {
        case 'h':
            return 0;
        case 'd':
            break;
        default:
            cout << "heartBeat when Arduino not ready\n";
    }

    readData = spiCom->CS0_transfer('M');
    if (readData != 'a') {
        cerr << "Protocol error!\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }
    usleep(1000);

    return 1;
}

heartBeat::heartBeat(SPICom *spiCom) {

    this->spiCom = spiCom;
}
