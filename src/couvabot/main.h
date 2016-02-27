//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_MAIN_H
#define COUVABOT_MAIN_H

#define RPI false

#ifndef RPI
//include Raspberry things (Raspicam library)
#else
    #define FRAME_WIDTH 640
    #define FRAME_HEIGHT 480
#endif


#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "src/camera.h"
#include "src/Blob.h"

using namespace cv;
using namespace std;

void createTrackbars();
int main(int argc, char **argv);

#endif //COUVABOT_MAIN_H
