//
// Created by clement on 12.03.2016.
//

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

    return 1;
}

arduinoData HeartBeat::getData() {

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

    return 1;

    return arduinoData();
}


int HeartBeat::start() {

    if ((millis() - time) < 100)    // Check the arduino every 10 ms in start mode.
        return 0;
    else
        time = millis();

    readData = spiCom->CS0_transfer('S');
    if (readData != 'h')
        cout << "Polled Arduino when not ready\n";

    readData = spiCom->CS0_transfer('E');   // EOT

    if (readData == 's')    // if there is an 's', we start !
        return 1;
    else
        return 0;
}
