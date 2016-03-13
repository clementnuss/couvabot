//
// Created by clement on 26.02.2016.
//

#include "camera.h"

#if RPI

#endif


RPiCam::RPiCam() {

    //Open camera
    cout << "Opening Camera...\n";
    if (!Camera.open()) {
        cerr << "Error opening camera\n";
    }

    cout << "Sleeping for 500 [ms]\n";
    usleep(500000);

    // Set properties
    Camera.set(CV_CAP_PROP_FORMAT, CV_8UC1);

    std::cout << "RPi camera initialized";

}

void RPiCam::read(cv::Mat &image) {
    Camera.grab();
    Camera.retrieve(image);
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

}
