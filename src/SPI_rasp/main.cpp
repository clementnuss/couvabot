#include <iostream>
#include <bcm2835.h>
#include <stdio.h>


using namespace std;


int main(int argc, char **argv)
{
    // If you call this, it will not actually access the GPIO
// Use for testing
//        bcm2835_set_debug(1);

    cout << "Test SPI Raspberry" << endl;

    if (!bcm2835_init())
        return 1;

    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536); // The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default

    // Send a byte to the slave and simultaneously read a byte back from the slave
    // If you tie MISO to MOSI, you should read back what was sent
    uint8_t send_data = 0x23;
    uint8_t read_data = bcm2835_spi_transfer(send_data);
    printf("Sent to SPI: 0x%02X. Read back from SPI: 0x%02X.\n", send_data, read_data);
    if (send_data != read_data)
        printf("Do you have the loopback from MOSI to MISO connected?\n");
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
