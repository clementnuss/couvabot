//
// Created by clement on 26.02.2016.
//

#include "camera.h"

RPiCam::RPiCam() {
    std::cout << "RPi camera initialized";
}

void RPiCam::read(cv::Mat &image) {
    //TODO: raspicam
}

WebCam::WebCam() {

    cap.open(0);

    if (!cap.isOpened())  // if not success, exit program
    {
        cerr << "Cannot open the web cam" << endl;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

}

void WebCam::read(cv::Mat &image) {
    cap.read(image);
}

void GenericCam::read(cv::Mat &image) {

}
