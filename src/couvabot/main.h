//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_MAIN_H
#define COUVABOT_MAIN_H

#define RPI false
#define CALIB true


#ifndef RPI
//include Raspberry things (Raspicam library)
#else
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240
#endif


#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "src/camera.h"
#include "src/Blob.h"

using namespace cv;
using namespace std;

bool initCam();

int main(int argc, char **argv);

struct HSVbounds {
    int hMin = 0, sMin = 0, vMin = 0,
            hMax = 180, sMax = 255, vMax = 255;
};

#endif //COUVABOT_MAIN_H
