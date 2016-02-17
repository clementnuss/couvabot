//
// Created by clement on 17.02.2016.
//

#ifndef SERVOSPICONTROL_SERVOSPICONTROL_H
#define SERVOSPICONTROL_SERVOSPICONTROL_H

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

void catchPuck(void);
void catchMoveG(void);
void catchIRCheck(int side);


#endif //SERVOSPICONTROL_SERVOSPICONTROL_H
