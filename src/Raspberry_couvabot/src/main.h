//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_MAIN_H
#define COUVABOT_MAIN_H

#define RPI true
#define CALIB true


#if RPI
//include Raspberry things (Raspicam library)
#define KEY_UP 65362
#define KEY_DOWN 65364
#define KEY_LEFT 65361
#define KEY_RIGHT 65363
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

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

void *capBlobs();

void *capImage();

int main(int argc, char **argv);

int loop();


struct HSVbounds {
    int hMin = 0, sMin = 0, vMin = 0,
            hMax = 180, sMax = 255, vMax = 255;
};

#endif //COUVABOT_MAIN_H
