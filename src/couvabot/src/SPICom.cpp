//
// Created by clement on 02.03.2016.
//

#include "SPICom.h"

bool bcm2835Initialized = false;

/*
SPICom::SPICom(bcm2835SPIClockDivider clock, bcm2835SPIChipSelect chipSelect) {

    cout << "Initialising SPI object" << endl;
    this->clock      = clock;
    this->chipSelect = chipSelect;

    if (!bcm2835Initialized)
        bcm2835_spi_begin();

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setClockDivider(clock);
    bcm2835_spi_chipSelect(chipSelect);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
}

uint8_t SPICom::spi_transfer(uint8_t send_data) {
    //TODO : check performance issue of continuously changing chipSelect & Clock
    bcm2835_spi_setClockDivider(clock);
    bcm2835_spi_chipSelect(chipSelect);
    bcm2835_spi_transfer(send_data);
}

*/