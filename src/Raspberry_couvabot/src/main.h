//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_MAIN_H
#define COUVABOT_MAIN_H

#include <iostream>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#if CALIB
    #include <opencv2/highgui/highgui.hpp>
#endif

#include "imageProcessing/camera.h"
#include "imageProcessing/Blob.h"
#include "robotConstants.h"

using namespace cv;
using namespace std;

void initCam();

void capBlobs();

void capImage();

int main(int argc, char **argv);

void * loop(void *threadArgs);

void * imgProc(void *threadArgs);

#endif //COUVABOT_MAIN_H
