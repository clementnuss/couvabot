//
// Created by clement on 10.03.2016.
//

#ifndef COUVABOT_ROBOTCONSTANTS_H
#define COUVABOT_ROBOTCONSTANTS_H


struct HSVbounds {
    int hMin = 0, sMin = 0, vMin = 0,
            hMax = 180, sMax = 255, vMax = 255;
};


#define RPI true
#define CALIB false
#define DEBUG false
#define WITE_BOARD false

#define BRAITENBERG_RATE 500


// Coefficients of the distance approximation polynomial
#define x_5 766.803126447968
#define x_4 -1229.155883963768
#define x_3 749.636516965590
#define x_2 -166.505522823536
#define x_1 60.240735082265
#define x_0 21.679281107097

#define RIGHT 1
#define LEFT 2
#define CLOSED 0

#if RPI
//include Raspberry things (Raspicam library)
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

#define KEY_UP 65362
#define KEY_DOWN 65364
#define KEY_LEFT 65361
#define KEY_RIGHT 65363

#else
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

#define KEY_UP 2490368
#define KEY_DOWN 2621440
#define KEY_LEFT 2424832
#define KEY_RIGHT 2555904
#endif

#define AXLE 0.4
#define WHEEL_SPEED 0.77519     // 1 / 1.29

#endif //COUVABOT_ROBOTCONSTANTS_H
