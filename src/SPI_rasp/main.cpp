#include <iostream>
#include <bcm2835.h>
#include <stdio.h>


using namespace std;

bool bcm2835Initialized = false;

class SPI {

    uint8_t send_data;
    uint8_t read_data;
    uint8_t chipSelect;
    bcm2835PWMClockDivider clock;

public:
    SPI(bcm2835PWMClockDivider clock, bcm2835SPIChipSelect chipSelect) {

        cout << "Initialising SPI object" << endl;
        this->clock = clock;
        this->chipSelect = chipSelect;

        if (!bcm2835Initialized)
            bcm2835_spi_begin();

        bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
        bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
        bcm2835_spi_setClockDivider(clock);
        bcm2835_spi_chipSelect(chipSelect);
        bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
    }

    uint8_t spi_transfer(uint8_t send_data) {
        //TODO : check performance issue of continuously changing chipSelect & Clock
        bcm2835_spi_setClockDivider(clock);
        bcm2835_spi_chipSelect(chipSelect);
        bcm2835_spi_transfer(send_data);
    }


};


int main(int argc, char **argv) {

    SPI arduinoSPI(BCM2835_SPI_CLOCK_DIVIDER_128, BCM2835_SPI_CS0);

    read_data = arduinoSPI.spi_transfer('c');
    printf("Sent to SPI: 0x%02X (char : %c). Read back from SPI: 0x%02X (char : %c).\n", send_data, send_data,
           read_data, read_data);

    read_data = arduinoSPI.spi_transfer('m');
    printf("Sent to SPI: 0x%02X (char : %c). Read back from SPI: 0x%02X (char : %c).\n", send_data, send_data,
           read_data, read_data);

    read_data = arduinoSPI.spi_transfer('b');
    printf("Sent to SPI: 0x%02X (char : %c). Read back from SPI: 0x%02X (char : %c).\n", send_data, send_data,
           read_data, read_data);

    read_data = arduinoSPI.spi_transfer(255);
    printf("Sent to SPI: 0x%02X (char : %c). Read back from SPI: 0x%02X (char : %c).\n", send_data, send_data,
           read_data, read_data);

    read_data = arduinoSPI.spi_transfer(f);
    printf("Sent to SPI: 0x%02X (char : %c). Read back from SPI: 0x%02X (char : %c).\n", send_data, send_data,
           read_data, read_data);

    read_data = arduinoSPI.spi_transfer('c');
    printf("Sent to SPI: 0x%02X (char : %c). Read back from SPI: 0x%02X (char : %c).\n", send_data, send_data,
           read_data, read_data);


    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

