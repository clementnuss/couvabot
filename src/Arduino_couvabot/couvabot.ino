#include "couvabot.h"

/***************************************************************
 Global Variables
  -receiveBuffer[] and dat are used to capture incoming data
   from the Raspberry Pi
  -marker is used as a pointer to keep track of the current
   position in the incoming data packet
***************************************************************/
uint8_t receiveBuffer[4];
uint8_t dat;
byte marker = 0;

char param1, param2, param3;
unsigned char state[2];

// MOTORS
Motor motorA(AIN1_GEAR, AIN2_GEAR, PWMA_GEAR);
Motor motorB(BIN1_GEAR, BIN2_GEAR, PWMB_GEAR);
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
            motorA.mStop();
            motorA.mStop();
            rotTimeBegin = 0;
            rotTimeEnd = 0;
            rotationEnable = 0;
        }
    }
}

void commandDecoder(void) {
    switch (receiveBuffer[0]) {
        case 'm':
            marker++;             // Motor movement
            SPDR = 'M'; // Acknowledge motor command
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
    }
}

int angleConvert(int rotAngle, int rotSpeed) {
    int travelTime;
    travelTime = floor((rotAngle / rotSpeed) * 1000);
}

void motorDecoder(void) {
    param1 = receiveBuffer[1];
    param2 = receiveBuffer[2];
    param3 = receiveBuffer[3];

    switch (param1) {
        case 'l':
            digitalWrite(STBY_GEAR, HIGH);  // l for left
            motorA.drive(param2, param3);
            break;
        case 'r':
            digitalWrite(STBY_GEAR, HIGH);  // r for right
            motorB.drive(param2, param3);
            break;
        case 'b':
            digitalWrite(STBY_GEAR, HIGH);  //both
            motorA.drive(param2, param3);
            motorB.drive(param2, !param3);
            break;
        case 'L':
            digitalWrite(STBY_GEAR, HIGH);  //rotation left (uppercase L)
            motorA.drive(param2, BACKWARD);
            motorB.drive(param2, FORWARD);
            rotTimeBegin = millis();
            rotationEnable = 1;
            rotTimeEnd = angleConvert(param3, param2);
            break;
        case 'R':
            digitalWrite(STBY_GEAR, HIGH);  //rotation right (uppercase R)
            motorA.drive(param2, FORWARD);
            motorB.drive(param2, BACKWARD);
            rotTimeBegin = millis();
            rotationEnable = 1;
            rotTimeEnd = angleConvert(param3, param2);
            break;
        case 's':
            digitalWrite(STBY_GEAR, LOW); // s for stop
            motorA.mStop();
            motorB.mStop();
            break;
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
            marker++;
            break;
        case 4:
            receiveBuffer[marker - 1] = SPDR;
            marker = 0;
            motorDecoder();
        default:
            SPDR = 'h'; // heartbeat on SPDR -- default char
    }

}

