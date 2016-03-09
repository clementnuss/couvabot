//
// Created by clement on 04.03.2016.
//

#include "mvmtController.h"
#include "../main.h"

using namespace mvmtCtrl;

mvmtController::mvmtController() {

    maxPWM = (int) ((7 * 255) / VBAT);

    if (RPI) {
        try {
            //first clock divider for Arduino
            spiCom = new SPICom(BCM2835_SPI_CLOCK_DIVIDER_128, BCM2835_SPI_CLOCK_DIVIDER_65536);
        } catch (string exception) {
            cerr << "Caught exception : " << exception;
        }
    }
}

bool mvmtController::move(int x, int y) {


    return false;
}

uint8_t mvmtController::getPWM(double p) {
    return (uint8_t) min((maxPWM * p), maxPWM);
}

bool mvmtController::arduiCommand(double pL, double pR) {
    bool leftForward = pL >= 0;
    bool rightForward = pR >= 0;

    int pwmL = getPWM(pL);
    int pwmR = getPWM(pR);
    cout << "pwmL: " << pwmL << " et pwmR: " << pwmR << "\n";

    readData = spiCom->CS0_transfer('H');
    if (readData != 'h') {
        cerr << "Protocol handshake error!\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }
    usleep(1000);

    readData = spiCom->CS0_transfer('M');
    if (readData != 'a') {
        cerr << "Protocol error!\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }
    usleep(1000);

    uint8_t controlByte = 0;
    if (pwmL > 1)
        controlByte = (uint8_t) ((pL >= 0) ? (0xF << 4) : (0xB << 4));
    if (pwmR > 1)
        controlByte = (uint8_t) ((pR >= 0) ? (controlByte | 0xF) : (controlByte | 0xB));

    printf("controlByte: 0x%1x \n", controlByte);

    readData = spiCom->CS0_transfer(controlByte);
    if (readData != 'm') {
        cerr << "Protocol error after having sent controlByte\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }
    usleep(1000);

    readData = spiCom->CS0_transfer(getPWM(pL));
    if (readData != 'o') {
        cerr << "Protocol error after having sent pL\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }
    usleep(1000);

    readData = spiCom->CS0_transfer(getPWM(pR));
    if (readData != 'k') {
        cerr << "Protocol error after having sent pR\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }
    usleep(1000);

    readData = spiCom->CS0_transfer('E');
    if (readData != 'o') {
        cerr << "Protocol error after having sent EOT\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }
    return true;
}

