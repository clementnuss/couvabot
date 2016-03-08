//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_CAMERA_H
#define COUVABOT_CAMERA_H

#include "../main.h"


class GenericCam {
public:
    virtual void read(cv::Mat &image) = 0;
};


class WebCam : public GenericCam {
    cv::VideoCapture cap;

public:
    WebCam();
    void read(cv::Mat &image);
};

class test : public GenericCam {

};

class RPiCam : public GenericCam {
public:
    RPiCam();
    void read(cv::Mat &image);
};


#endif //COUVABOT_CAMERA_H
