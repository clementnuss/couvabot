//
// Created by Clément Nussbaumer on 10.02.16.
//
// Inspired by Kyle Hounslow
//

#include "main.h"
#include "src/detectObjects.h"

int CAMERA_ANGLE = 0;
int hMax, hMin, sMin, sMax, vMin, vMax;

void createTrackbars() {
    namedWindow("Trackbars", 0);

    hMin = sMin = vMin = 0;
    hMax = 180;
    sMax = vMax = 255;

    createTrackbar("min_H", "Trackbars", &hMin, hMax, nullptr);
    createTrackbar("max_H", "Trackbars", &hMax, hMax, nullptr);
    createTrackbar("min_S", "Trackbars", &sMin, sMax, nullptr);
    createTrackbar("max_S", "Trackbars", &sMax, sMax, nullptr);
    createTrackbar("min_V", "Trackbars", &vMin, vMax, nullptr);
    createTrackbar("max_V", "Trackbars", &vMax, vMax, nullptr);
}

int main(int argc, char **argv) {

    GenericCam *cam;

    if (RPI) {
        cam = new RPiCam();
    }
    else {
        cam = new WebCam();
    }
    Mat img, hsv, filtered;

    createTrackbars();

    vector<Blob> redBlobs, greenBlobs;
    int frameCnt = 0;

    while (1) {

        cam->read(img);

        cvtColor(img, hsv, COLOR_BGR2HSV);

        inRange(hsv, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), filtered);

        Mat erodeElement = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));

        erode(filtered, filtered, erodeElement);
        erode(filtered, filtered, erodeElement);

        dilate(filtered, filtered, dilateElement);
        dilate(filtered, filtered, dilateElement);

        cout << "Frame # " << frameCnt++ << "\n";

        if (detectObjects(redBlobs, filtered, RED)) {
            for (int i = 0; i < redBlobs.size(); i++) {
                cout << "Blob x :  " << redBlobs[i].getPosX()
                << " y : " << redBlobs[i].getPosY() <<
                " ; aire : " << redBlobs[i].getArea() << "\t";
            }
        }



        //show frames
        imshow("filtered", filtered);
        imshow("camera", img);
        imshow("HSV image", hsv);


        if (waitKey(500) == 27)
            break;

    }

    return 0;
}