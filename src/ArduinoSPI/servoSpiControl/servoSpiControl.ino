#include <Servo.h>

// DRIVING GEARS
#define FORWARD         0
#define BACKWARD        1
#define BOTH            2
#define MOTOR_A         0
#define MOTOR_B         1
#define STOP            3
#define MAX_SPEED       255
#define CW              0
#define CCW             1
#define ROT_L           4
#define ROT_R           5

// SREVO MANEUVRES
#define B_CLOSE         0
#define B_OPEN          180
#define B_LAPSE         5
#define B_ROLL_LAPSE    1000
#define B_ROLL_SPEED    150
#define B_RELEASE       3
#define B_ROLL          4
#define B_ROLLING       5
#define B_RECLOSE       6
#define F_LAPSE         3
#define F_CATCH_G       3
#define F_CATCH_R       4
#define F_IR_G          5
#define F_IR_R          6
#define F_BELT          7
#define F_PULL          8
#define F_PULL_LAPSE    750
#define F_LIFT          9
#define F_LIFT_LAPSE    8000
#define M_MID           90
#define M_RED           60
#define M_GREEN         120
#define L_OPEN          180
#define L_CLOSE         90
#define L_PUSH          80
#define L_SAFE          150
#define R_OPEN          0
#define R_CLOSE         90
#define R_PUSH          100
#define R_SAFE          30

// SENSORS IO
#define IR_CATCH_G      0
#define IR_CATCH_R      1
#define IR_DIST         512
#define IR_CATCH_LAPSE  1000

//-------------------PINS TO CHANGE----------------------
#define STBY_GEAR       24 //standby, can be digital pin

// Motor A
#define PWMA_GEAR       10 //Speed control, PWM pin
#define AIN1_GEAR       25 //Direction, can be digital pin
#define AIN2_GEAR       26 //Direction

// Motor B
#define PWMB_GEAR       9 
#define BIN1_GEAR       27 
#define BIN2_GEAR       28 

// Conveyor belt motor
#define PWM_BELT        1
#define AIN1_BELT       1
#define AIN2_BELT       1

// Servos
#define SERVO_FRONT_L   1  // Servo PWM pins
#define SERVO_FRONT_M   1
#define SERVO_FRONT_R   1
#define SERVO_BACK      1

// Buttons and IRs
#define CATCH_BUTTON    1  // Button digital pin
#define IR_ENABLE       1  // IR digital pin
#define IR_CATCH_G      A1 // IR analog pin
#define IR_CATCH_R      A1

//-------------------------------------------------------

class Motor
{
        // Class Member Variables
        // These are initialized at startup
        int motorPin1;                       // Direction pins
        int motorPin2;
        int motorPWMPin;                     // Speed pin

        volatile int     velocity = 0;
        volatile boolean orient1  = LOW;
        volatile boolean orient2  = LOW;

    public:
        Motor(int pin1, int pin2, int pwm){
            motorPin1   = pin1;
            motorPin2   = pin2;
            motorPWMPin = pwm;

            pinMode(motorPin1,   OUTPUT);    // Can be digital output
            pinMode(motorPin2,   OUTPUT);
            pinMode(motorPWMPin, OUTPUT);    // Must be analog output
        }

        void drive(int vel, int orientation){
            velocity = vel;
            orient1  = LOW;
            orient2  = HIGH;
    
            if(orientation == BACKWARD){
                orient1 = HIGH;
                orient2 = LOW;
            }
    
            digitalWrite(motorPin1,   orient1);
            digitalWrite(motorPin2,   orient2);
            analogWrite (motorPWMPin, velocity);
        }

        void mStop(void){
            digitalWrite(motorPin1,   LOW);
            digitalWrite(motorPin2,   LOW);
            analogWrite (motorPWMPin, 0);
    
            velocity = 0;
            orient1  = LOW;
            orient2  = LOW;
        }
};

class ServoM
{
        // Class Member Variables
        // These are initialized at startup
        Servo servo;
        int   resetPos;                      // Servo position at startup

    public:
        volatile int anglePos;               // Position of servo at any time
        
        ServoM(int rPos){
            resetPos = rPos;
            anglePos = rPos;
        }
    
        void Attach(int pin){
            servo.attach(pin);
        }
    
        void Detach(){
            servo.detach();
        }
    
        void reset(void){
            servo.write(resetPos);
            anglePos = resetPos;
        }
    
        void writePos(int angle){
            servo.write(angle);
            anglePos = angle;
        }

        void updatePos(void){
            servo.write(anglePos);
        }
};

// COMMUNICATION
unsigned char receiveBuffer[4];     // Capture incoming data from RPi
unsigned char dat;
byte          marker = 0;           // Data-byte packet pointer
char          param1,               // Received parameters from RPi
              param2, 
              param3;
unsigned char state[2];

// MOTORS
Motor motorA    (AIN1_GEAR, AIN2_GEAR, PWMA_GEAR);  // Object declaration
Motor motorB    (BIN1_GEAR, BIN2_GEAR, PWMB_GEAR);
Motor motorBelt (AIN1_BELT, AIN2_BELT, PWM_BELT);
volatile unsigned long motorRotTimeBegin   = 0;
volatile unsigned long motorRotTimeEnd     = 0;
bool                   motorRotationEnable = 0;

// SERVOS
ServoM frontServoL (L_OPEN);                        // Object declaration
ServoM frontServoR (R_OPEN);
ServoM frontServoM (M_MID);
ServoM backServo   (B_CLOSE);
bool                   servoReleasePucks = 0;
bool                   servoCatch        = 0;
volatile int           B_STATE           = 0;
volatile int           F_STATE           = 0;
volatile unsigned long bServoTimeBegin   = 0;
volatile unsigned long fServoTimeBegin   = 0;


// FUNCTION DECLARATION ----------------------------------
int angleConvert    (int rotAngle, int rotSpeed);
void servoPrepare   (int angle);
void catchIRCeck    (int side);
void commandDecoder (void);
void motorDecoder   (void);
void spiHandler     (void);
void freePucks      (void);
void motorRotation  (void);
void catchMoveR     (void);
void catchMoveL     (void);

// SETUP -------------------------------------------------
void setup() {
    pinMode(MISO, OUTPUT);               // Enable SPI
    SPCR |= _BV(SPE);
  
    pinMode(STBY_GEAR,    OUTPUT);       // Gears setup

    pinMode(CATCH_BUTTON, INPUT);        // Sensors setup
    pinMode(IR_ENABLE,    OUTPUT);
  
    frontServoL.Attach (SERVO_FRONT_L);  // Servo setup
    frontServoM.Attach (SERVO_FRONT_M);
    frontServoR.Attach (SERVO_FRONT_R);
    backServo.Attach   (SERVO_BACK);
  
    frontServoL.reset ();
    frontServoR.reset ();
    frontServoM.reset ();
    backServo.reset   ();
  
}

// LOOP --------------------------------------------------
void loop() {
    if((SPSR & (1 << SPIF)) != 0){ 
        spiHandler();
    }
  
    if(motorRotationEnable){       // Bot is doing a rotation on itself
        motorRotation();
    }
  
    if(servoCatch){                // Bot will catch a puck in front of it
        catchPuck();
    }
  
    if(servoReleasePucks){         // Bot open back doors
        freePucks();
    }
}

// FUNCTIONS ---------------------------------------------
void catchPuck(void){
    switch(F_STATE){
        case(F_CATCH_G): if((millis() - fServoTimeBegin) >= F_LAPSE){  // Catch a puck
                             catchMoveG();
                         }
                         if((frontServoL.anglePos == L_PUSH)  &&
                            (frontServoR.anglePos == R_CLOSE) &&
                            (frontServoM.anglePos == M_MID)){
                             F_STATE = F_IR_G;
                             digitalWrite(IR_ENABLE, HIGH);
                             fServoTimeBegin = 0;
                         }
                         break;
        case(F_CATCH_R): if((millis() - fServoTimeBegin) >= F_LAPSE){
                             catchMoveR();
                         }
                         if((frontServoL.anglePos == L_CLOSE) &&
                            (frontServoR.anglePos == R_PUSH)  &&
                            (frontServoM.anglePos == M_MID)){
                             digitalWrite(IR_ENABLE, HIGH);
                             F_STATE         = F_IR_R;
                             fServoTimeBegin = millis();
                         }
                         break;
        case(F_IR_G):    catchIRCheck(IR_CATCH_G);                     // Wait until IR detects
        case(F_IR_R):    catchIRCheck(IR_CATCH_R);                     // puck
        case(F_BELT):    motorBelt.drive(MAX_SPEED, FORWARD);          // Pull the puck up
                         fServoTimeBegin = millis();
                         F_STATE         = F_PULL;
                         break;
        case(F_PULL):    if((millis() - fServoTimeBegin) >= F_PULL_LAPSE){
                             frontServoL.reset();
                             frontServoR.reset();
                             frontServoM.reset();
                             F_STATE = F_LIFT;
                             // READY TO MOVE //////////////////////////////////////
                         }
                         break;
        case(F_LIFT):    if(!digitalRead(CATCH_BUTTON)){               // Puck is captured
                             motorBelt.mStop();
                             F_STATE    = 0;
                             servoCatch = 0;
                             // JOB FINISHED ///////////////////////////////////////
                         }else if((millis() - fServoTimeBegin) >= F_LIFT_LAPSE){
                             // ERROR //////////////////////////////////////////////
                         }
    }
}

void catchIRCheck(int side){
    if(side == IR_CATCH_G){
        if(analogRead(IR_CATCH_G) < IR_DIST){    // Puck in front of IR
            digitalWrite(IR_ENABLE, LOW);
            F_STATE         = F_BELT;
            fServoTimeBegin = 0;
        }else if((millis() - fServoTimeBegin) >= IR_CATCH_LAPSE){
            digitalWrite(IR_ENABLE, LOW);
            frontServoL.reset();
            frontServoR.reset();
            frontServoM.reset();
            // ERROR //////////////////////////////////////////////////
        }
    }else{
        if(analogRead(IR_CATCH_R) < IR_DIST){
            digitalWrite(IR_ENABLE, LOW);
            F_STATE         = F_BELT;
            fServoTimeBegin = 0;
        }else if((millis() - fServoTimeBegin) >= IR_CATCH_LAPSE){
            digitalWrite(IR_ENABLE, LOW);
            frontServoL.reset();
            frontServoR.reset();
            frontServoM.reset();
            // ERROR //////////////////////////////////////////////////
        }
    }
}

void catchMoveG(void){                  // Servo maneuvre to catch the puck
    if(frontServoL.anglePos > L_SAFE){
        frontServoL.anglePos -= 1;
        frontServoL.updatePos();
        fServoTimeBegin = millis();
    }else{
        if(frontServoL.anglePos > L_PUSH){
            frontServoL.anglePos -= 1;
            frontServoL.updatePos();
        }
        if(frontServoR.anglePos < R_CLOSE){
            frontServoR.anglePos += 1;
            frontServoR.updatePos();
        }
        if(frontServoM.anglePos > M_MID){
            frontServoM.anglePos -= 1;
            frontServoM.updatePos();
        }
        fServoTimeBegin = millis();
    }
}

void catchMoveR(void){
    if(frontServoR.anglePos < R_SAFE){
        frontServoR.anglePos += 1;
        frontServoR.updatePos();
        fServoTimeBegin = millis();
    }else{
        if(frontServoL.anglePos > L_CLOSE){
            frontServoL.anglePos -= 1;
            frontServoL.updatePos();
        }
        if(frontServoR.anglePos < R_PUSH){
            frontServoR.anglePos += 1;
            frontServoR.updatePos();
        }
        if(frontServoM.anglePos > M_MID){
            frontServoM.anglePos -= 1;
            frontServoM.updatePos();
        }
        fServoTimeBegin = millis();
    }
}

void motorRotation(void){
    if((millis() - motorRotTimeBegin) >= motorRotTimeEnd){
            motorA.mStop();
            motorA.mStop();
            motorRotTimeBegin   = 0;
            motorRotTimeEnd     = 0;
            motorRotationEnable = 0;
    }
}

void freePucks(void){
    switch(B_STATE){
        case(B_RELEASE): if((millis() - bServoTimeBegin) >= B_LAPSE){  // Open doors, gently
                             if(backServo.anglePos < B_OPEN){
                                 backServo.anglePos += 1;
                                 backServo.updatePos();
                                 bServoTimeBegin = millis();
                             }else{
                                 B_STATE = B_ROLL;
                             }
                         }
                         break;
        case(B_ROLL):    motorA.drive(B_ROLL_SPEED, FORWARD);          // drive forward to 
                         motorB.drive(B_ROLL_SPEED, BACKWARD);         // free the pucks
                         bServoTimeBegin = millis();
                         B_STATE         = B_ROLLING;
                         break;
        case(B_ROLLING): if((millis() - bServoTimeBegin) >= B_ROLL_LAPSE){
                             bServoTimeBegin = 0;
                             B_STATE         = B_RECLOSE;
                         }
                         break;
        case(B_RECLOSE): motorA.mStop   ();                            // Close doors, finished
                         motorB.mStop   ();
                         backServo.reset();
                         B_STATE           = 0;
                         servoReleasePucks = 0;
    }
}

void commandDecoder(void){
    switch(receiveBuffer[0]){
        case 'm': marker++;              // Motor movement
                  break;
        case 'r': marker  = 0;           // Red puck
                  servoPrepare(M_RED);
                  F_STATE = F_CATCH_R;
                  break;
        case 'g': marker  = 0;           // Green puck
                  servoPrepare(M_GREEN);
                  F_STATE = F_CATCH_G;
                  break;
        case 'k': marker          = 0;   // Catch the puck
                  servoCatch      = 1;
                  fServoTimeBegin = millis();
                  break;
        case 'f': marker            = 0; // Release all pucks
                  servoReleasePucks = 1;
                  bServoTimeBegin   = millis();
                  B_STATE           = B_RELEASE;
                  
    }
}

int angleConvert(int rotAngle, int rotSpeed){          // Finds out how much time is needed to
    int travelTime;                                    // perform a rotation
    travelTime = (int)floor((rotAngle/rotSpeed)*1000);
    return travelTime;
}

void motorDecoder(void){          // Input from SPI communication with RPi
    param1 = receiveBuffer[1];
    param2 = receiveBuffer[2];
    param3 = receiveBuffer[3];
  
    switch(param1){
        case 'l': digitalWrite(STBY_GEAR, HIGH);  // l for left
                  motorA.drive(param2, param3);
                  break;
        case 'r': digitalWrite(STBY_GEAR, HIGH);  // r for right
                  motorB.drive(param2, param3);
                  break;
        case 'b': digitalWrite(STBY_GEAR, HIGH);  // both
                  motorA.drive(param2, param3);
                  motorB.drive(param2, !param3);
                  break;
        case 'L': digitalWrite(STBY_GEAR, HIGH);  // rotation left (uppercase L)
                  motorA.drive(param2, BACKWARD);
                  motorB.drive(param2, FORWARD);
                  motorRotTimeBegin   = millis();
                  motorRotationEnable = 1;
                  motorRotTimeEnd     = angleConvert(param3, param2);
                  break;
        case 'R': digitalWrite(STBY_GEAR, HIGH);  // rotation right (uppercase R)
                  motorA.drive(param2, FORWARD);
                  motorB.drive(param2, BACKWARD);
                  motorRotTimeBegin   = millis();
                  motorRotationEnable = 1;
                  motorRotTimeEnd     = angleConvert(param3, param2);
                  break;
        case 's': digitalWrite(STBY_GEAR, LOW);   // s for stop
                  motorA.mStop();
                  motorB.mStop();
                  break;
    }
}

void spiHandler(void){       // Stores the incoming byte packets from SPI
    switch (marker){
        case 0: dat = SPDR;
                if(dat == 'c'){
                    SPDR = 'a';
                    marker++;
                } 
                break;    
        case 1: receiveBuffer[marker-1] = SPDR;
                commandDecoder();
                break;
        case 2: receiveBuffer[marker-1] = SPDR;
                marker++;
                break;
        case 3: receiveBuffer[marker-1] = SPDR;
                marker++;
                break;
        case 4: receiveBuffer[marker-1] = SPDR;
                marker=0;
                motorDecoder();
    }

}

void servoPrepare(int angle){      // Presets the middle-servo for the coming puck
    frontServoM.writePos(angle);
}

