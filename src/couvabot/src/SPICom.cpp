//
// Created by clement on 02.03.2016.
//

#include <unistd.h>
#include "SPICom.h"

bool bcm2835Initialized = false;
#ifndef RPI


/**
 * Constructor of an SPICom instance. Initializes the CS0 settings by default
 */
SPICom::SPICom(bcm2835SPIClockDivider clockCS0, bcm2835SPIClockDivider clockCS1) {

    cout << "Initialising SPI object" << endl;
    this->clockCS0   = clockCS0;
    this->clockCS1   = clockCS1;
    this->chipSelect = BCM2835_SPI_CS0;

    if (!bcm2835Initialized)
        bcm2835_spi_begin();

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setClockDivider(clockCS0);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);


    uint8_t readData = CS0_transfer('h'); //Send heartbeat char 'h'
    if (readData != 'I') {
        cerr << "Arduino doesnt have 'I' in SPDR .. waiting 300ms";
        usleep(300); // Wait 300ms if no comm with Arduino
    }
    readData = CS0_transfer('h'); //Send heartbeat char 'h'
    if (readData != 'I') {
        cerr << "Arduino again doesnt have 'I' in SPDR .. Aborting communication";
        throw "SPI initializazion error";
    }

}

uint8_t SPICom::CS0_transfer(uint8_t send_data) {
    if (chipSelect != BCM2835_SPI_CS0) {
        bcm2835_spi_setClockDivider(clockCS0);
        bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
        chipSelect = BCM2835_SPI_CS0;
    }

    return bcm2835_spi_transfer(send_data);
}

uint8_t SPICom::CS1_transfer(uint8_t send_data) {
    if (chipSelect != BCM2835_SPI_CS1) {
        bcm2835_spi_setClockDivider(clockCS1);
        bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
        chipSelect = BCM2835_SPI_CS1;
    }

    return bcm2835_spi_transfer(send_data);
}

#endif