//
// Created by clement on 02.03.2016.
//

#include <unistd.h>
#include "SPICom.h"
#include "localTime.h"
#include "../robotConstants.h"

bool bcm2835Initialized = false;

#if RPI

/**
 * Constructor of an SPICom instance. Initializes the CS0 settings by default
 */
SPICom::SPICom(bcm2835SPIClockDivider clockCS0, bcm2835SPIClockDivider clockCS1) {

    cout << "Initialising SPI object" << endl;
    this->clockCS0 = clockCS0;
    this->clockCS1 = clockCS1;
    this->chipSelect = BCM2835_SPI_CS0;

    if (!bcm2835_init())
        cerr << "Error during initialization of bcm2835 library!!";


    if (!bcm2835Initialized)
        bcm2835_spi_begin();

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setClockDivider(clockCS0);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);

    uint8_t readData = CS0_transfer('H'); //Send RPi heartbeat char 'H'
    if (readData != 'h') {
        cout << "Arduino doesnt have 'h' in SPDR .. waiting 3ms" << "\n";
        cerr << "instead it has: " << readData << "\n";
        usleep(3000); // Wait 3ms if no comm with Arduino

        readData = CS0_transfer('H'); //Send RPI heartbeat char 'H'
        if (readData != 'h') {
            cerr << "Arduino again doesnt have 'h' in SPDR .. Aborting communication" << "\n";
            cerr << "instead it has: " << readData << "\n";
            throw "SPI initializazion error";
        }
    }


    readData = CS0_transfer('E'); //Send RPi EOT
    cout << "Arduino communication initialized\n";

    time = micros();
}

uint8_t SPICom::CS0_transfer(uint8_t send_data) {
    if ((micros() - time) < 50) // Check that we haven't send anything for 50 [us]
        usleep((micros() - time));

    if (DEBUG)
        cout << "Data sent: " << send_data << "\n";

    time = micros();

    if (chipSelect != BCM2835_SPI_CS0) {
        bcm2835_spi_setClockDivider(clockCS0);
        bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
        chipSelect = BCM2835_SPI_CS0;
    }
    uint8_t read_Data =  bcm2835_spi_transfer(send_data);
    if (DEBUG)
        cout << "read data: " << read_Data << "\n";
    return read_Data;
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

#if !RPI

uint8_t SPICom::CS0_transfer(uint8_t send_data) {

    cout << "Data to send " << (char) send_data << "[char] or " << send_data << " [hex]\n";
    return 'D'; //Default dummy data
}

SPICom::SPICom(bcm2835SPIClockDivider clockCS0, bcm2835SPIClockDivider clockCS1) {
    cerr << "Dummy constructor";
}

#endif
