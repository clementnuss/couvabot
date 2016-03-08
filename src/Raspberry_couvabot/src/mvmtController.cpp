//
// Created by clement on 04.03.2016.
//

#include "mvmtController.h"
#include "../main.h"

using namespace mvmtCtrl;

mvmtController::mvmtController() {

    maxPWM = (int) ((8 * 255) / VBAT);

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
    return (uint8_t) (maxPWM * p);
}

bool mvmtController::arduiCommand(double pL, double pR) {
    bool leftForward = pL >= 0;
    bool rightForward = pR >= 0;

    readData = spiCom->CS0_transfer('M');
    if (readData != 'h') {
        cerr << "Protocol error!\n";
        return false;
    }
    usleep(100);

    uint8_t controlByte = 0;
    if (abs(pL) >= 0.02f)
        controlByte = (uint8_t) (pL ? (0xF << 4) : (0xB << 4));
    if (abs(pL) >= 0.02f)
        controlByte = (uint8_t) (pL ? (0xF) : (0xB));

    readData = spiCom->CS0_transfer(controlByte);
    if (readData != 'm') {
        cerr << "Protocol error after having sent controlByte\n";
        cerr << "Received" << readData << "\n";
        return false;
    }
    usleep(100);

    spiCom->CS0_transfer(getPWM(pL));
    if (readData != 'o') {
        cerr << "Protocol error after having sent pL\n";
        cerr << "Received" << readData << "\n";
        return false;
    }
    usleep(100);

    spiCom->CS0_transfer(getPWM(pR));
    if (readData != 'k') {
        cerr << "Protocol error after having sent pR\n";
        cerr << "Received" << readData << "\n";
        return false;
    }
    usleep(100);

    spiCom->CS0_transfer(getPWM('E'));
    if (readData != 'o') {
        cerr << "Protocol error after having sent EOT\n";
        cerr << "Received" << readData << "\n";
        return false;
    }
    return true;
}

