//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_MAIN_H
#define COUVABOT_MAIN_H

#define RPI false
#define CALIB true
#define DEBUG false
#define WITE_BOARD true

#define x_5 766.803126447968
#define x_4 -1229.155883963768
#define x_3 749.636516965590
#define x_2 -166.505522823536
#define x_1 60.240735082265
#define x_0 21.679281107097

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


#include <iostream>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#if CALIB
#include <opencv2/highgui/highgui.hpp>
#endif

#include "imageProcessing/camera.h"
#include "imageProcessing/Blob.h"

using namespace cv;
using namespace std;

bool initCam();

void capBlobs();

void capImage();

int main(int argc, char **argv);

void * loop(void *threadArgs);

void * imgProc(void *threadArgs);

struct HSVbounds {
    int hMin = 0, sMin = 0, vMin = 0,
            hMax = 180, sMax = 255, vMax = 255;
};

#endif //COUVABOT_MAIN_H
