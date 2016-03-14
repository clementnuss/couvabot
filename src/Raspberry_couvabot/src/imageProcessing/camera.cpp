//
// Created by clement on 26.02.2016.
//

#include "camera.h"

#if RPI

#endif


RPiCam::RPiCam() {
#if RPI
    // Set properties
    camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
    camera.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
    camera.set(CV_CAP_PROP_EXPOSURE, 100);

    //Open camera
    cout << "Opening camera...\n";
    if (!camera.open()) {
        cerr << "Error opening camera\n";
    }

    std::cout << "RPi camera initialized\n";
#endif
}


void RPiCam::read(cv::Mat &image) {
#if RPI

    camera.grab();
    camera.retrieve(image);

#endif
}

WebCam::WebCam() {

    cap.open(0);

    if (!cap.isOpened())  // if not success, exit program
    {
        cerr << "Cannot open the web cam\n";
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

}

void WebCam::read(cv::Mat &image) {
    cap.read(image);
}

void GenericCam::read(cv::Mat &image) {
    cerr << "WARNING! Using dummy camera!!\n";
}

double project(int x, int y) {


}