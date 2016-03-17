#include "couvabot.h"
#include "src/ServoM.h"

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
byte sendMarker = 0;
bool sendingData;

int timeout = 0;
uint8_t state[2];

// MOTORS
Motor motorLeft(AIN1_GEAR, AIN2_GEAR, PWMA_GEAR);
Motor motorRight(BIN1_GEAR, BIN2_GEAR, PWMB_GEAR);
Motor motorBelt(AIN1_BELT, AIN2_BELT, PWM_BELT);

// SERVOS
ServoM frontServoL(L_OPEN);                        // Object declaration
ServoM frontServoR(R_OPEN);
ServoM frontServoM(M_MID);
ServoM backServoL(B_CLOSE);
ServoM backServoR(B_CLOSE);

bool servoReleasePucks = 0;
bool servoCatch = 0;
volatile int B_STATE = 0;
volatile int F_STATE = 0;
volatile unsigned long bServoTimeBegin = 0;
volatile unsigned long fServoTimeBegin = 0;

// SENSORS
bool startLoop = false;
int startSignal = 0;

void setup() {
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE);
    initSPI(); // Initialize SPDR to 'h' -- heartbeat char, default char on SPDR.

    pinMode(STBY_GEAR, OUTPUT);  // Must be added in setup
    pinMode(STBY_BELT, OUTPUT);

    frontServoL.Attach(SERVO_FRONT_L);  // Servo setup
    frontServoM.Attach(SERVO_FRONT_M);
    frontServoR.Attach(SERVO_FRONT_R);
    backServoL.Attach(SERVO_BACK_L);
    backServoR.Attach(SERVO_BACK_R);

    frontServoL.reset();
    frontServoR.reset();
    frontServoM.reset();
    backServoL.reset();
    backServoR.reset();
}

void loop() {
    if ((SPSR & (1 << SPIF)) != 0) {
        spiHandler();
    }

    if (startLoop) {
        startSignal = 0;
        for (int k = 0; k < 10; k++) {
            startSignal += analogRead(A3) + analogRead(A4);
        }
        startSignal = startSignal / 20;
        if (startSignal < 800) {
            sendingData = 1;
            startLoop = false;
            SPDR = 's'; // start
        } else {
            SPDR = 'w'; // wait
        }
    }

    if (servoCatch) catchPuck();

    if (servoReleasePucks) freePucks();
}

void spiHandler() {

    switch (marker) {
        case 0:
            if (SPDR == 'H') {
                SPDR = 'a'; // Acknowledges spi communication start
                marker++;
            } else if (SPDR == 'D') {       //When the raspberry asks for Data, the arduino serves it
                marker = 10;
                sendData();
            } else if (SPDR == 'S'){
                startLoop = true;
                initSPI();
            } else
                initSPI();
            break;
        case 6:
            initSPI();      //Resets SPI communication stack
        case 10:
            sendData();
            break;
        default:
            receiveBuffer[marker - 1] = SPDR;
            commandDecoder();

    }
}

void sendData() {
    switch (sendMarker) {
        case 0:
            //SPDR = analogRead();
            sendMarker++;
            break;
        case 1:
            if (SPDR == 'D') {

            }
        default:
            initSPI();
    }

}

void initSPI() {
    SPDR = 'h'; // heartbeat on SPDR -- default char
    for (int i = 0; i < 5; ++i) {
        receiveBuffer[i] = 0;
    }
    marker = 0;
    sendMarker = 0;
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
                default:
                    initSPI();
            }
            break;
        case 'L':
            initSPI();
            servoPrepare(M_LEFT);
            break;
        case 'R':
            initSPI();
            servoPrepare(M_RIGHT);
            break;
        case 'C':
            servoCatch = 1;         // Catch the puck
            initSPI();
            break;
        case 'f':
            marker = 0;           // Release all pucks
            servoReleasePucks = 1;
            break;

        default:
            initSPI();
            break;
    }
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

    digitalWrite(STBY_GEAR, HIGH);
    motorLeft.drive(pL, FORWARD);

    digitalWrite(STBY_GEAR, HIGH);
    motorRight.drive(pR, FORWARD);

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
            motorRight.drive(pR, FORWARD);
            break;
        case 0xB:  // Backward
            digitalWrite(STBY_GEAR, HIGH);
            motorRight.drive(pR, BACKWARD);
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

void catchPuck(void) {
    switch (F_STATE) {
        case (F_CATCH_L):
            if ((millis() - fServoTimeBegin) >= F_LAPSE) {  // Catch a puck
                catchMoveL();
            }
            if ((frontServoL.anglePos == L_PUSH) &&
                (frontServoR.anglePos == R_CLOSE) &&
                (frontServoM.anglePos == M_MID)) {
                F_STATE = F_BELT;
                fServoTimeBegin = 0;
            }
            break;
        case (F_CATCH_R):
            if ((millis() - fServoTimeBegin) >= F_LAPSE) {
                catchMoveR();
            }
            if ((frontServoL.anglePos == L_CLOSE) &&
                (frontServoR.anglePos == R_PUSH) &&
                (frontServoM.anglePos == M_MID)) {
                fServoTimeBegin = millis();
                F_STATE = F_BELT;
            }
            break;
        case (F_BELT):
            frontServoL.reset();
            frontServoR.reset();
            frontServoM.reset();
            digitalWrite(STBY_BELT, HIGH);
            motorBelt.drive(B_ROLL_SPEED, FORWARD);          // Pull the puck up
            fServoTimeBegin = millis();
            F_STATE = F_PULL;
            break;
        case (F_PULL):
            if ((millis() - fServoTimeBegin) >= F_PULL_LAPSE) {

                F_STATE = F_LIFT;
                // READY TO MOVE //////////////////////////////////////
            }
            break;
        case (F_LIFT):              // Puck is captured
            motorBelt.mStop();
            digitalWrite(STBY_BELT, LOW);
            F_STATE = 0;
            servoCatch = 0;
            // JOB FINISHED ///////////////////////////////////////
            //}else if((millis() - fServoTimeBegin) >= F_LIFT_LAPSE){
            // ERROR //////////////////////////////////////////////
            //}
    }
}

void catchMoveL(void) {                  // Servo maneuver to catch the puck
    if (frontServoL.anglePos > L_SAFE) {
        frontServoL.anglePos -= 1;
        frontServoL.updatePos();
        fServoTimeBegin = millis();
    } else {
        if (frontServoL.anglePos > L_PUSH) {
            frontServoL.anglePos -= 1;
            frontServoL.updatePos();
        }
        if (frontServoR.anglePos < R_CLOSE) {
            frontServoR.anglePos += 1;
            frontServoR.updatePos();
        }
        if (frontServoM.anglePos > M_MID) {
            frontServoM.anglePos -= 1;
            frontServoM.updatePos();
        }
        fServoTimeBegin = millis();
    }
}

void catchMoveR(void) {
    if (frontServoR.anglePos < R_SAFE) {
        frontServoR.anglePos += 1;
        frontServoR.updatePos();
        fServoTimeBegin = millis();
    } else {
        if (frontServoL.anglePos > L_CLOSE) {
            frontServoL.anglePos -= 1;
            frontServoL.updatePos();
        }
        if (frontServoR.anglePos < R_PUSH) {
            frontServoR.anglePos += 1;
            frontServoR.updatePos();
        }
        if (frontServoM.anglePos > M_MID) {
            frontServoM.anglePos -= 1;
            frontServoM.updatePos();
        }
        fServoTimeBegin = millis();
    }
}

void freePucks(void) {
    switch (B_STATE) {
        case (B_RELEASE):
            if ((millis() - bServoTimeBegin) >= B_LAPSE) {  // Open doors, gently
                if (backServoL.anglePos > B_OPEN) {
                    backServoL.anglePos -= 1;
                    backServoL.updatePos();
                    bServoTimeBegin = millis();
                } else {
                    B_STATE = B_ROLL;
                }
            }
            break;
        case (B_ROLL):
            digitalWrite(STBY_GEAR, HIGH);
            motorLeft.drive(40, FORWARD);          // drive forward to
            motorRight.drive(40, BACKWARD);         // free the pucks
            bServoTimeBegin = millis();
            B_STATE = B_ROLLING;
            break;
        case (B_ROLLING):
            if ((millis() - bServoTimeBegin) >= B_ROLL_LAPSE) {
                bServoTimeBegin = 0;
                B_STATE = B_RECLOSE;
            }
            break;
        case (B_RECLOSE):
            digitalWrite(STBY_GEAR, LOW);
            motorLeft.mStop();                            // Close doors, finished
            motorRight.mStop();
            backServoL.reset();
            backServoR.reset();
            B_STATE = 0;
            servoReleasePucks = 0;
        default:
            return;
    }
}

void servoPrepare(int angle) {      // Presets the middle-servo for the coming puck
    frontServoM.writePos(angle);
}