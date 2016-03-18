//
// Created by clement on 18.03.2016.
//

#ifndef COUVABOT_BRAITENBERG_H
#define COUVABOT_BRAITENBERG_H

#include "../physIface/arduinoComm.h"
typedef ardCom::gearsPower gearsPower;

void braiten(gearsPower *powers, ardCom::sensorsData sens);


#endif //COUVABOT_BRAITENBERG_H
