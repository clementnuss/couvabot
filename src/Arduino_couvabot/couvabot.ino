#include "couvabot.h"

/***************************************************************
 Global Variables
  -receiveBuffer[] and dat are used to capture incoming data
   from the Raspberry Pi
  -marker is used as a pointer to keep track of the current
   position in the incoming data packet
***************************************************************/
uint8_t receiveBuffer[5];
uint8_t dat;
byte marker = 0;
bool dataWaiting;

int timeout = 0;
uint8_t state[2];

// MOTORS
Motor motorLeft(AIN1_GEAR, AIN2_GEAR, PWMA_GEAR);
Motor motorRight(BIN1_GEAR, BIN2_GEAR, PWMB_GEAR);
volatile unsigned int rotTimeBegin = 0;
volatile unsigned int rotTimeEnd = 0;
bool rotationEnable = 0;

void setup() {
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE);
    initSPI(); // Initialize SPDR to 'h' -- heartbeat char, default char on SPDR.

    pinMode(STBY_GEAR, OUTPUT);  // Must be added in setup
}

void loop() {
    if ((SPSR & (1 << SPIF)) != 0) {
        spiHandler();
    }

    if (rotationEnable) {
        if ((millis() - rotTimeBegin) > rotTimeEnd) {
            motorLeft.mStop();
            motorLeft.mStop();
            rotTimeBegin = 0;
            rotTimeEnd = 0;
            rotationEnable = 0;
        }
    }
}

void spiHandler() {

    switch (marker) {
        case 0:
            if (SPDR == 'H') {
                if (dataWaiting) {
                    sendData();
                }
                else {
                    SPDR = 'h'; // Acknowledge spi communication start
                    marker++;
                }
            } else
                initSPI();
            break;
        case 6:
            initSPI();      //Resets SPI communication stack
        default:
            receiveBuffer[marker - 1] = SPDR;
            commandDecoder();

    }
}

void sendData() {

}

void initSPI() {
    SPDR = 'h'; // heartbeat on SPDR -- default char
    for (int i = 0; i < 5; ++i) {
        receiveBuffer[i] = 0;
    }
    marker = 0;
}

void commandDecoder() {
    switch (receiveBuffer[0]) {
        case 'M':
            switch (marker) {
                case 1:
                    marker++;
                    SPDR = 'm';     // Acknowledge motor command
                    break;
                case 2:
                    marker++;
                    SPDR = 'o';
                    break;
                case 3:
                    marker++;
                    SPDR = 'k';
                    break;
                case 4:
                    marker++;
                    SPDR = 'o';
                    break;
                case 5:
                    if (SPDR == 'E')
                        motorDecoder();
                    else
                        initSPI();

                default:
                    initSPI();
            }
            break;
        case 'L':
            initSPI();
            //servoPrepare(left container);
            break;
        case 'R':
            initSPI();
            //servoPrepare(right container);
            break;
        case 'k':
            marker = 0;           // Catch the puck
            //catchPuck();
            break;
        case 'f':
            marker = 0;           // Release all pucks
            //freePucks();
            break;

        default:
            initSPI();
            break;
    }
}

int angleConvert(int rotAngle, int rotSpeed) {
    int travelTime;
    travelTime = floor((rotAngle / rotSpeed) * 1000);
}

void motorDecoder() {
    uint8_t ctrlLeft = (receiveBuffer[1] & (0xF << 4)) >> 4;
    uint8_t ctrlRight = (receiveBuffer[1] & 0xF);

    uint8_t pL = receiveBuffer[2];
    uint8_t pR = receiveBuffer[3];

    if (receiveBuffer[1] == 0) {
        analogWrite(STBY_GEAR, LOW);
        return;
    }

    switch (ctrlLeft) {
        case 0xF:  // Forward
            digitalWrite(STBY_GEAR, HIGH);
            motorLeft.drive(pL, FORWARD);
            break;
        case 0xB:  // Backward
            digitalWrite(STBY_GEAR, HIGH);
            motorLeft.drive(pL, BACKWARD);
            break;
        case 0x0:
            digitalWrite(STBY_GEAR, LOW);
            motorLeft.mStop();
            break;

        default:
            timeout++;
            break;
    }

    switch (ctrlRight) {
        case 0xF:  // Forward
            digitalWrite(STBY_GEAR, HIGH);
            motorRight.drive(pL, FORWARD);
            break;
        case 0xB:  // Backward
            digitalWrite(STBY_GEAR, HIGH);
            motorRight.drive(pL, BACKWARD);
            break;
        case 0x0:
            digitalWrite(STBY_GEAR, LOW);
            motorRight.mStop();
            break;

        default:
            timeout++;
            break;
    }


    /*
         * TODO: Ancien code, a réutiliser pour implémenter les "virages"
         *
        case 'L':
            digitalWrite(STBY_GEAR, HIGH);  //rotation left (uppercase L)
            motorLeft.drive(param2, BACKWARD);
            motorRight.drive(param2, FORWARD);
            rotTimeBegin = millis();
            rotationEnable = 1;
            rotTimeEnd = angleConvert(param3, param2);
            break;
        case 'R':
            digitalWrite(STBY_GEAR, HIGH);  //rotation right (uppercase R)
            motorLeft.drive(param2, FORWARD);
            motorRight.drive(param2, BACKWARD);
            rotTimeBegin = millis();
            rotationEnable = 1;
            rotTimeEnd = angleConvert(param3, param2);
            break;
        case 's':
            digitalWrite(STBY_GEAR, LOW); // s for stop
            motorLeft.mStop();
            motorRight.mStop();
            break;
            */
}

