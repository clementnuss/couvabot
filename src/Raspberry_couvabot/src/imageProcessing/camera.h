//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_CAMERA_H
#define COUVABOT_CAMERA_H

#include <opencv2/highgui/highgui.hpp>
#include "../../../raspicam/src/raspicam_cv.h"

using namespace std;
using namespace cv;


class GenericCam {
public:
    virtual void read(cv::Mat &image) = 0;
};

/*
 * Any webcam connected
 */

class WebCam : public GenericCam {
    cv::VideoCapture cap;

public:
    WebCam();
    void read(cv::Mat &image);
};

/*
 * RPi Cam
 */

class RPiCam : public GenericCam {
#if RPI
    raspicam::RaspiCam_Cv camera;
#endif
public:
    RPiCam();
    void read(cv::Mat &image);
};

double project(int x, int y);


#endif //COUVABOT_CAMERA_H
