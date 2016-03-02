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
    bcm2835SPIClockDivider clock;

public:
    SPICom(bcm2835SPIClockDivider clock, bcm2835SPIChipSelect chipSelect);
    uint8_t spi_transfer(uint8_t send_data);

};
#endif //COUVABOT_SPICOM_H
