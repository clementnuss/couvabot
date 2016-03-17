//
// Created by clement on 26.02.2016.
//

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>


using namespace std;
using namespace cv;

#include "../robotConstants.h"
#include "detectObjects.h"

//Minimum/maximum detection settings
const int MIN_AREA = 10 * 10;
const int MAX_AREA = 500;
const int MAX_NUM_OBJECTS = 30;


bool detectObjects(vector<Blob> &blobs, Mat binaryImage, int colour) {

    vector<vector<Point>> contours; //Stores the contours as a succession of Points
    contours.reserve(MAX_NUM_OBJECTS);

    Mat tmp;
    binaryImage.copyTo(tmp);

    findContours(tmp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    blobs.clear();

    if (contours.size() > 0 && contours.size() < MAX_NUM_OBJECTS) {
        blobs.reserve(contours.size());

        for (int i = 0; i < contours.size(); ++i) {

            Moments mom(cv::moments((Mat) contours[i], false));

            int area = (int) mom.m00;      //m00

            if (area > MIN_AREA && area < MAX_AREA) {
                int x = (int) (mom.m10 / area);    //m10
                int y = (int) (mom.m01 / area);    //m01
                Blob tmpBlob(x, y, area, colour);
                blobs.push_back(tmpBlob);
                cout << "Added a blob\n";
            }
        }
        return true;

    } else {
        if (contours.size() == 0) {
            if (DEBUG)
                cout << "No object detected";
        }
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

bool filterBoard(HSVbounds hsvBounds, Mat binaryImage, RotatedRect &board) {

    vector<vector<Point>> contours; //Stores the contours as a succession of Points
    contours.reserve(10);

    Mat tmp;
    binaryImage.copyTo(tmp);

    findContours(tmp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    int biggest = -1;
    int area = -1;

    if (contours.size() > 0 && contours.size() < 40) {
        for (int i = 0; i < contours.size(); ++i) {

            Moments mom = moments(contours[i], false);

            if (mom.m00 > area) {
                area = (int) mom.m00;
                biggest = i;
            }
        }

        board = minAreaRect(contours[biggest]);
        return true;

    } else {
        if (contours.size() == 0) {
            cout << "No board detected!! \n";
        } else {
            cout << "Board calibration error!\n";
        }
        return false;
    }
}

void imgProcess(HSVbounds hsvBounds, Mat &hsv, Mat &filtered) {
    inRange(hsv,
            Scalar(hsvBounds.hMin, hsvBounds.sMin, hsvBounds.vMin),
            Scalar(hsvBounds.hMax, hsvBounds.sMax, hsvBounds.vMax),
            filtered);

    Mat erodeElement = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
    Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));

    erode(filtered, filtered, erodeElement);
    erode(filtered, filtered, erodeElement);

    dilate(filtered, filtered, dilateElement);
    dilate(filtered, filtered, dilateElement);
}

/** decide whether point p is in the ROI.
*** The ROI is a rotated rectange whose 4 corners are stored in roi[]
**/
bool isInROI(Point p, Point2f roi[]) {
    double pro[4];
    for (int i = 0; i < 4; ++i) {
        pro[i] = computeProduct(p, roi[i], roi[(i + 1) % 4]);
    }
    return pro[0] * pro[2] < 0 && pro[1] * pro[3] < 0;
}

/** function pro = kx-y+j, take two points a and b,
*** compute the line argument k and j, then return the pro value
*** so that can be used to determine whether the point p is on the left or right
*** of the line ab
**/
double computeProduct(Point p, Point2f a, Point2f b) {
    double k = (a.y - b.y) / (a.x - b.x);
    double j = a.y - k * a.x;
    return k * p.x - p.y + j;
}