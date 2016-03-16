//
// Created by clement on 16.03.2016.
//

#include "ServoM.h"

ServoM::ServoM(int rPos){
    resetPos = rPos;
    anglePos = rPos;
}

void ServoM::Attach(int pin){
    servo.attach(pin);
}

void ServoM::Detach(){
    servo.detach();
}

void ServoM::reset(void){
    servo.write(resetPos);
    anglePos = resetPos;
}

void ServoM::writePos(int angle){
    servo.write(angle);
    anglePos = angle;
}

void ServoM::updatePos(void){
    servo.write(anglePos);
}