//
// Created by clement on 04.03.2016.
//

#include "mvmtController.h"
#include "../main.h"

using namespace mvmtCtrl;

mvmtController::mvmtController() {

    maxPWM = (8 * 255) / VBAT;

    if (!RPI) {
        try {
            //first clock divider for Arduino
            arduiCom = new SPICom(BCM2835_SPI_CLOCK_DIVIDER_128, BCM2835_SPI_CLOCK_DIVIDER_65536);
        } catch (string exception){
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
    bool leftForward  = pL >= 0;
    bool rightForward = pR >= 0;

    readData = arduiCom->CS0_transfer('c');
    if (readData != 'A') {
        cerr << "Protocol error!";
        return false;
    }

    readData = arduiCom->CS0_transfer(getPWM(pL));
    //TODO: ajouter les autres commandes, selon protocole de communication

    return true;

}

