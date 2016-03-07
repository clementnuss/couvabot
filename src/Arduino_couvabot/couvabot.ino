#include "couvabot.h"

/***************************************************************
 Global Variables
  -receiveBuffer[] and dat are used to capture incoming data
   from the Raspberry Pi
  -marker is used as a pointer to keep track of the current
   position in the incoming data packet
***************************************************************/
uint8_t receiveBuffer[3];
uint8_t dat;
byte marker = 0;

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
    SPDR = 'h'; // Initialize SPDR to 'h' -- heartbeat char, default char on SPDR.

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

void spiHandler(void) {
    switch (marker) {
        case 0:
            dat = SPDR;
            if (dat == 'c') {
                SPDR = 'A'; // Acknowledge spi communication start
                marker++;
            }
            break;
        case 1:
            receiveBuffer[marker - 1] = SPDR;
            commandDecoder();
            break;
        case 2:
            receiveBuffer[marker - 1] = SPDR;
            marker++;
            break;
        case 3:
            receiveBuffer[marker - 1] = SPDR;
            marker = 0;
            motorDecoder();
        default:
            SPDR = 'h'; // heartbeat on SPDR -- default char
            for (int i = 0; i < 3; ++i) {
                receiveBuffer[i] = 0;
            }
    }
}

void commandDecoder(void) {
    switch (receiveBuffer[0]) {
        case 'M':
            marker++;             // Motor movement
            SPDR = 'm'; // Acknowledge motor command
            break;
        case 'r':
            marker = 0;           // Red puck
            //servoPrepare(M_RED);
            break;
        case 'g':
            marker = 0;           // Green puck
            //servoPrepare(M_GREEN);
            break;
        case 'k':
            marker = 0;           // Catch the puck
            //catchPuck();
            break;
        case 'f':
            marker = 0;           // Release all pucks
            //freePucks();
        default:
            SPDR = 'h';
            break;
    }
}

int angleConvert(int rotAngle, int rotSpeed) {
    int travelTime;
    travelTime = floor((rotAngle / rotSpeed) * 1000);
}

void motorDecoder(void) {
    uint8_t ctrlLeft = (receiveBuffer[0] & (0xF << 4)) >> 4;
    uint8_t ctrlRight = (receiveBuffer[0] & 0xF);

    uint8_t pL = receiveBuffer[1];
    uint8_t pR = receiveBuffer[2];

    switch (ctrlLeft) {
        case 0xF:  // Forward
            digitalWrite(STBY_GEAR, HIGH);  // l for left
            motorLeft.drive(pL, FORWARD);
            break;
        case 0xB:  // Backward
            digitalWrite(STBY_GEAR, HIGH);  // l for left
            motorLeft.drive(pL, BACKWARD);
            break;
        case 0x0:
            digitalWrite(STBY_GEAR, LOW);  //both
            motorLeft.mStop();
            break;

        default:
            timeout++;
            break;
    }

    switch (ctrlRight) {
        case 0xF:  // Forward
            digitalWrite(STBY_GEAR, HIGH);  // l for left
            motorRight.drive(pL, FORWARD);
            break;
        case 0xB:  // Backward
            digitalWrite(STBY_GEAR, HIGH);  // l for left
            motorRight.drive(pL, BACKWARD);
            break;
        case 0x0:
            digitalWrite(STBY_GEAR, LOW);  //both
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

