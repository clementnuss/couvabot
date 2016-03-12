//
// Created by clement on 12.03.2016.
//

#include <unistd.h>
#include "HeartBeat.h"
#include "localTime.h"

HeartBeat::HeartBeat(SPICom *spiCom) {

    this->spiCom = spiCom;
}

int HeartBeat::pingArduino() {

    if ((millis() - time) < 200)    // Ping the arduino every 200 ms.
        return 0;
    else
        time = millis();

    readData = spiCom->CS0_transfer('H');
    switch (readData) {
        case 'h':
            return 0;
        case 'd':
            break;
        default:
            cout << "HeartBeat when Arduino not ready\n";
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

arduinoData HeartBeat::getData() {
    return arduinoData();
}


int HeartBeat::start() {

    if ((millis() - time) < 10)    // Check the arduino every 10 ms in start mode.
        return 0;
    else
        time = millis();

    readData = spiCom->CS0_transfer('H');
    switch (readData) {
        case 'h':
            return 0;
        case 'd':
            break;
        default:
            cout << "HeartBeat when Arduino not ready\n";
    }

    readData = spiCom->CS0_transfer('D'); // Acknowledge data transmit mode
    if (readData == 's')    // if there is an 's', we start !
        return 1;

    readData = spiCom->CS0_transfer('E'); // End transmission

    return 0;
}
