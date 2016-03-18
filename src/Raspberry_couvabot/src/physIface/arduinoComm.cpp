//
// Created by clement on 04.03.2016.
//

#include <math.h>
#include <unistd.h>
#include "arduinoComm.h"
#include "../robotConstants.h"
#include "localTime.h"

using namespace ardCom;

arduinoComm::arduinoComm(SPICom *spiCom, double vBat) {
    maxPWM = (int) ((2.5 * 255) / vBat);
    this->spiCom = spiCom;

    pollTime = 0;
}


double arduinoComm::powerToSpeed(double power) {
    return WHEEL_SPEED * power;
}

uint8_t arduinoComm::getPWM(double p) {
    return (uint8_t) ((maxPWM * p) > maxPWM ? maxPWM : maxPWM * p);
}

bool arduinoComm::gearsCommand(gearsPower power) {

    int pwmL = getPWM(fabs(power.pL));
    int pwmR = getPWM(fabs(power.pR));
    cout << "pwmL: " << pwmL << " et pwmR: " << pwmR << "\n";

    readData = spiCom->CS0_transfer('H');
    if (readData != 'h') {
        cerr << "Protocol handshake error!\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    readData = spiCom->CS0_transfer('M');
    if (readData != 'a') {
        cerr << "Protocol error!\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    uint8_t controlByte = 0;
    if (pwmL > 1)
        controlByte = (uint8_t) ((power.pL >= 0) ? (0xF << 4) : (0xB << 4));
    if (pwmR > 1)
        controlByte = (uint8_t) ((power.pR >= 0) ? (controlByte | 0xF) : (controlByte | 0xB));

    readData = spiCom->CS0_transfer(controlByte);
    if (readData != 'm') {
        cerr << "Protocol error after having sent controlByte\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    readData = spiCom->CS0_transfer(getPWM(power.pL));
    if (readData != 'o') {
        cerr << "Protocol error after having sent pL\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    readData = spiCom->CS0_transfer(getPWM(power.pR));
    if (readData != 'k') {
        cerr << "Protocol error after having sent pR\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    readData = spiCom->CS0_transfer('E');
    if (readData != 'o') {
        cerr << "Protocol error after having sent EOT\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    return true;
}

bool arduinoComm::catchPuck() {

    readData = spiCom->CS0_transfer('H');
    if (readData != 'h') {
        cerr << "Protocol handshake error!\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    spiCom->CS0_transfer('C');

    return true;
}


bool arduinoComm::prepareLeft() {

    readData = spiCom->CS0_transfer('H');
    if (readData != 'h') {
        cerr << "Protocol handshake error!\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    spiCom->CS0_transfer('L');

    return true;
}


bool arduinoComm::prepareRight() {

    readData = spiCom->CS0_transfer('H');
    if (readData != 'h') {
        cerr << "Protocol handshake error!\n";
        cerr << "Received: " << readData << "\n";
        return false;
    }

    spiCom->CS0_transfer('R');

    return true;
}


sensorsData arduinoComm::getData() {
    sensorsData sens;
    sens.valid = false;

    if ((millis() - pollTime) < 100)    // Check the arduino every 10 ms in start mode.
        return sens;
    else
        pollTime = millis();

    readData = spiCom->CS0_transfer('D');   // Ask for Data
    if (readData != 'h') {
        cout << "Arduino was not ready to serve Data\n";
        return sens;
    }

    usleep(1000);   // Wait 1 [ms] before getting the values

    for (int i = 0; i < 8; ++i) {
        sens.values[i] = spiCom->CS0_transfer('R');  // Random byte, to get data from Arduino
    }

    spiCom->CS0_transfer('E');  // send EOT
    sens.valid = true;
    return sens;
}


int arduinoComm::start() {

    if ((millis() - pollTime) < 100)    // Check the arduino every 10 ms in start mode.
        return 0;
    else
        pollTime = millis();

    readData = spiCom->CS0_transfer('S');
    if (readData != 'h')
        cout << "Polled Arduino when not ready\n";

    readData = spiCom->CS0_transfer('E');   // EOT

    if (readData == 's')    // if there is an 's', we start !
        return 1;
    else
        return 0;
}
