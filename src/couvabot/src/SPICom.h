//
// Created by clement on 02.03.2016.
//

#ifndef COUVABOT_SPICOM_H
#define COUVABOT_SPICOM_H

#include <iostream>
#include <bcm2835.h>

using namespace std;


class SPICom {

    uint8_t                send_data;
    uint8_t                read_data;
    uint8_t                chipSelect;
    bcm2835SPIClockDivider clockCS0, clockCS1;


public:
    SPICom(bcm2835SPIClockDivider clockCS0, bcm2835SPIClockDivider clockCS1);

    uint8_t CS0_transfer(uint8_t send_data);

    uint8_t CS1_transfer(uint8_t send_data);

};

#endif //COUVABOT_SPICOM_H
