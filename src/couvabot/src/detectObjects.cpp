//
// Created by clement on 26.02.2016.
//

#include "detectObjects.h"

//Minimum/maximum detection settings
const int MIN_AREA        = 25 * 25;
const int MAX_AREA        = FRAME_WIDTH * FRAME_HEIGHT / 4;
const int MAX_NUM_OBJECTS = 30;

bool detectObjects(vector<Blob> &blobs, Mat binaryImage, int colour) {

    vector<vector<Point>> contours; //Stores the contours as a succession of Points
    contours.reserve(30);

    Mat tmp;
    binaryImage.copyTo(tmp);

    findContours(tmp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    if (contours.size() > 0 && contours.size() < MAX_NUM_OBJECTS) {
        blobs.reserve(contours.size());

        for (int i = 0; i < contours.size(); ++i) {
            //TODO: optimize moments method to only compute m00, m01 and m10
            Moments moment(moments((Mat) contours[i]));

            int area = (int) moment.m00;

            if (area > MIN_AREA && area < MAX_AREA) {
                int  x = (int) (moment.m10 / area);
                int  y = (int) (moment.m01 / area);
                Blob tmpBlob(x, y, area, colour);
                blobs.push_back(tmpBlob);
            }
        }
        return true;

    } else {
        if (contours.size() == 0)
            cout << "No object detected";
        else
            cout << "Too much object detected -- adjust filter";

        return false;
    }
}

void createTrackbars(HSVbounds &bounds, string winName) {
    namedWindow(winName, 0);

    createTrackbar("min_H", winName, &bounds.hMin, bounds.hMax, nullptr);
    createTrackbar("max_H", winName, &bounds.hMax, bounds.hMax, nullptr);

    createTrackbar("min_S", winName, &bounds.sMin, bounds.sMax, nullptr);
    createTrackbar("max_S", winName, &bounds.sMax, bounds.sMax, nullptr);

    createTrackbar("min_V", winName, &bounds.vMin, bounds.vMax, nullptr);
    createTrackbar("max_V", winName, &bounds.vMax, bounds.vMax, nullptr);
}

void imgProc(HSVbounds hsvBounds, Mat &hsv, Mat &filtered) {
    inRange(hsv,
            Scalar(hsvBounds.hMin, hsvBounds.sMin, hsvBounds.vMin),
            Scalar(hsvBounds.hMax, hsvBounds.sMax, hsvBounds.vMax),
            filtered);

    Mat erodeElement  = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));

    erode(filtered, filtered, erodeElement);
    erode(filtered, filtered, erodeElement);

    dilate(filtered, filtered, dilateElement);
    dilate(filtered, filtered, dilateElement);
}