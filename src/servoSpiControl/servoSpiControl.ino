#include <avr/io.h>
#include <Arduino.h>
#include <Servo.h>

#include "servoSpiControl.h"
#include "SpiConstants.h"

class Motor
{
        // Class Member Variables
        // These are initialized at startup
        int motorPin1;                       // Direction pins
        int motorPin2;
        int motorPWMPin;                     // Speed pin

        volatile int     velocity;
        volatile boolean orient1;
        volatile boolean orient2;

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

int counter = 0;

// SETUP -------------------------------------------------
void setup() {
    //pinMode(MISO, OUTPUT);               // Enable SPI
    //SPCR |= _BV(SPE);
  
    pinMode(STBY_GEAR,    OUTPUT);       // Gears setup
    pinMode(STBY_BELT,    OUTPUT);

    pinMode(CATCH_BUTTON, INPUT);        // Sensors setup
    //pinMode(IR_ENABLE,    OUTPUT);
  
    frontServoL.Attach (SERVO_FRONT_L);  // Servo setup
    frontServoM.Attach (SERVO_FRONT_M);
    frontServoR.Attach (SERVO_FRONT_R);
    backServo.Attach   (SERVO_BACK_R);
  
    frontServoL.reset ();
    frontServoR.reset ();
    frontServoM.reset ();
    backServo.reset   ();

}

// LOOP --------------------------------------------------
void loop() {
  /*
    if((SPSR & (1 << SPIF)) != 0){ 
        spiHandler();
    }
    */
    if(!digitalRead(CATCH_BUTTON)){
      counter++;
      //counter = 1;
      if(counter == 3){
        F_STATE = F_CATCH_G;
        fServoTimeBegin = millis();
        
      }
      if(counter == 4){
        bServoTimeBegin   = millis();
        B_STATE = B_RELEASE;
      }
      if(counter >= 5){
        counter = 0;
      }
      delay(300);
    }
    
      if(counter == 1){
          frontServoM.writePos(M_GREEN);
          digitalWrite(STBY_GEAR, HIGH);
          motorA.drive(30, FORWARD);
          motorB.drive(30, BACKWARD);
          delay(400);
          digitalWrite(STBY_GEAR, LOW);
          motorA.mStop();
          motorB.mStop();
          counter = 2;
          //return;
      }
      
      else if(counter == 3){
          if(motorRotationEnable){       // Bot is doing a rotation on itself
              motorRotation();
          }
                      // Bot will catch a puck in front of it
          catchPuck();
          //return;
      }
    
     else if(counter == 4){
                // Bot open back doors
            freePucks();
            //return;
      }
}

// FUNCTIONS ---------------------------------------------
void catchPuck(void){
    switch(F_STATE){
        case(F_CATCH_G): 
            //delay(10);
            if((millis() - fServoTimeBegin) >= F_LAPSE){  // Catch a puck
                catchMoveG();
            }
            if((frontServoL.anglePos == L_PUSH)  &&
               (frontServoR.anglePos == R_CLOSE) &&
               (frontServoM.anglePos == M_MID)){
                //F_STATE = F_IR_G;
                //digitalWrite(IR_ENABLE, HIGH);
                F_STATE = F_BELT;
                fServoTimeBegin = 0;
            }
            break;
        case(F_CATCH_R): 
            delay(10);
            //if((millis() - fServoTimeBegin) >= F_LAPSE){
                catchMoveR();
            //}
            if((frontServoL.anglePos == L_CLOSE) &&
               (frontServoR.anglePos == R_PUSH)  &&
               (frontServoM.anglePos == M_MID)){
                //digitalWrite(IR_ENABLE, HIGH);
                //F_STATE         = F_IR_R;
                fServoTimeBegin = millis();
                F_STATE = F_BELT;
            }
            break;
        case(F_IR_G):    catchIRCheck(IR_CATCH_G);                     // Wait until IR detects
        case(F_IR_R):    catchIRCheck(IR_CATCH_R);                     // puck
        case(F_BELT):    
                frontServoL.reset();
                frontServoR.reset();
                frontServoM.reset();
                digitalWrite(STBY_BELT, HIGH);
            motorBelt.drive(B_ROLL_SPEED, FORWARD);          // Pull the puck up
            fServoTimeBegin = millis();
            F_STATE         = F_PULL;
            break;
        case(F_PULL):    if((millis() - fServoTimeBegin) >= F_PULL_LAPSE){
                
                F_STATE = F_LIFT;
                // READY TO MOVE //////////////////////////////////////
            }
            break;
        case(F_LIFT):    //if(!digitalRead(CATCH_BUTTON)){               // Puck is captured
                motorBelt.mStop();
                digitalWrite(STBY_BELT, LOW);
                F_STATE    = 0;
                servoCatch = 0;
                // JOB FINISHED ///////////////////////////////////////
            //}else if((millis() - fServoTimeBegin) >= F_LIFT_LAPSE){
                // ERROR //////////////////////////////////////////////
            //}
    }
}

void catchIRCheck(int side) {
    if(side == IR_CATCH_G){
      Serial.println(5);
        if(analogRead(IR_CATCH_G) < IR_DIST){    // Puck in front of IR
            digitalWrite(IR_ENABLE, LOW);
            F_STATE         = F_BELT;
            fServoTimeBegin = 0;
            Serial.println(2);
        }else if((millis() - fServoTimeBegin) >= IR_CATCH_LAPSE){
            digitalWrite(IR_ENABLE, LOW);
            frontServoL.reset();
            frontServoR.reset();
            frontServoM.reset();
            Serial.println(3);
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

void catchMoveG(void){                  // Servo maneuver to catch the puck
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
                             if(backServo.anglePos > B_OPEN){
                                 backServo.anglePos -= 1;
                                 backServo.updatePos();
                                 bServoTimeBegin = millis();
                             }else{
                                 B_STATE = B_ROLL;
                             }
                         }
                         break;
        case(B_ROLL):    digitalWrite(STBY_GEAR, HIGH);
                         motorA.drive(40, FORWARD);          // drive forward to 
                         motorB.drive(40, BACKWARD);         // free the pucks
                         bServoTimeBegin = millis();
                         B_STATE         = B_ROLLING;
                         break;
        case(B_ROLLING): if((millis() - bServoTimeBegin) >= B_ROLL_LAPSE){
                             bServoTimeBegin = 0;
                             B_STATE         = B_RECLOSE;
                         }
                         break;
        case(B_RECLOSE): digitalWrite(STBY_GEAR, LOW);
                         motorA.mStop   ();                            // Close doors, finished
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

