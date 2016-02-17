#include <avr/io.h>

#define FORWARD     0
#define BACKWARD    1
#define BOTH        2
#define MOTOR_A     0
#define MOTOR_B     1
#define STOP        3
#define MAX_SPEED   255
#define CW          0
#define CCW         1
#define ROT_L       4
#define ROT_R       5

//-------------------PINS TO CHANGE----------------------
#define STBY_GEAR  24 //standby

//Motor A
#define PWMA_GEAR  10 //Speed control 
#define AIN1_GEAR  25 //Direction
#define AIN2_GEAR  26 //Direction

//Motor B
#define PWMB_GEAR  9 //Speed control
#define BIN1_GEAR  27 //Direction
#define BIN2_GEAR  28 //Direction
//-------------------------------------------------------

class Motor {
    // Class Member Variables
    // These are initialized at startup
    int motorPin1;
    int motorPin2;
    int motorPWMPin;

    volatile int velocity;
    volatile boolean orient1;
    volatile boolean orient2;

public:
    Motor(int pin1, int pin2, int pwm) {
        motorPin1 = pin1;
        motorPin2 = pin2;
        motorPWMPin = pwm;
        velocity = 0;
        orient1 = LOW;
        orient2 = LOW;

        pinMode(motorPin1, OUTPUT);   // Can be digital output
        pinMode(motorPin2, OUTPUT);
        pinMode(motorPWMPin, OUTPUT); // Must be analog output
    }

    void drive(int vel, int orientation) {
        velocity = vel;
        orient1 = LOW;
        orient2 = HIGH;

        if (orientation == BACKWARD) {
            orient1 = HIGH;
            orient2 = LOW;
        }

        digitalWrite(motorPin1, orient1);
        digitalWrite(motorPin2, orient2);
        analogWrite(motorPWMPin, velocity);
    }

    void mStop(void) {
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
        analogWrite(motorPWMPin, 0);

        velocity = 0;
        orient1 = LOW;
        orient2 = LOW;
    }
};

/***************************************************************
 Global Variables
  -receiveBuffer[] and dat are used to capture incoming data
   from the Raspberry Pi
  -marker is used as a pointer to keep track of the current
   position in the incoming data packet
***************************************************************/
unsigned char receiveBuffer[4];
unsigned char dat;
byte marker = 0;

char param1, param2, param3;
unsigned char state[2];

// MOTORS
Motor motorA(AIN1_GEAR, AIN2_GEAR, PWMA_GEAR);
Motor motorB(BIN1_GEAR, BIN2_GEAR, PWMB_GEAR);
volatile unsigned long rotTimeBegin = 0;
volatile unsigned long rotTimeEnd = 0;
bool rotationEnable = 0;

// Function declaration ----------------------------------
int angleConvert(int rotAngle, int rotSpeed);

void commandDecoder(void);

void motorDecoder(void);

void spiHandler(void);

void setup() {
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE);

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
                SPDR = 'a';
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
    }

}

