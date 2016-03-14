//
// Created by clement on 26.02.2016.
//

#include "detectObjects.h"

//Minimum/maximum detection settings
const int MIN_AREA = 25 * 25;
const int MAX_AREA = FRAME_WIDTH * FRAME_HEIGHT / 4;
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

            int moments[3];
            momentsOfOrder1((Mat) contours[i], moments);

            int area = moments[0];      //m00

            if (area > MIN_AREA && area < MAX_AREA) {
                int x = (moments[1] / area);    //m10
                int y = (moments[2] / area);    //m01
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

bool filterBoard(HSVbounds hsvBounds, Mat binaryImage, RotatedRect &board) {

    vector<vector<Point>> contours; //Stores the contours as a succession of Points
    contours.reserve(5);

    Mat tmp;
    binaryImage.copyTo(tmp);

    findContours(tmp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    int biggest = -1;
    int area = -1;

    if (contours.size() > 0 && contours.size() < 40) {
        for (int i = 0; i < contours.size(); ++i) {

            int moments[3];
            momentsOfOrder1((Mat) contours[i], moments);
            if (moments[0] > area) {
                area = moments[0];
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

    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat dilateElement = getStructuringElement(MORPH_RECT, Size(6, 6));

    erode(filtered, filtered, erodeElement);
    erode(filtered, filtered, erodeElement);

    dilate(filtered, filtered, dilateElement);
    dilate(filtered, filtered, dilateElement);
}

void momentsOfOrder1(const cv::Mat &img, int *moments) {
    cv::Size size = img.size();
    int x, y;
    int mom[3] = {0, 0, 0};

    for (y = 0; y < size.height; y++) {
        const int *ptr = (const int *) (img.data + y * img.step);
        int x0 = 0, x1 = 0;

        for (x = 0; x < size.width; x++) {
            int p = ptr[x];
            int xp = x * p, xxp;

            x0 += p;
            x1 += xp;
        }

        int py = y * x0;

        mom[2] += py;             // m01
        mom[1] += x1;             // m10
        mom[0] += x0;             // m00


        for (int i = 0; i < 3; ++i) {
            moments[i] = mom[0];
        }
    }
}


/** decide whether point p is in the ROI.
*** The ROI is a rotated rectange whose 4 corners are stored in roi[]
**/
bool isInROI(Point p, Point2f roi[])
{
    double pro[4];
    for(int i=0; i<4; ++i)
    {
        pro[i] = computeProduct(p, roi[i], roi[(i+1)%4]);
    }
    return pro[0] * pro[2] < 0 && pro[1] * pro[3] < 0;
}

/** function pro = kx-y+j, take two points a and b,
*** compute the line argument k and j, then return the pro value
*** so that can be used to determine whether the point p is on the left or right
*** of the line ab
**/
double computeProduct(Point p, Point2f a, Point2f b)
{
    double k = (a.y-b.y) / (a.x-b.x);
    double j = a.y - k*a.x;
    return k*p.x - p.y + j;
}