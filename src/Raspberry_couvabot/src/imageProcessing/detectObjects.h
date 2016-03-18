//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_OBJDETECT_H
#define COUVABOT_OBJDETECT_H

#include <opencv2/core/core.hpp>

#include "../robotConstants.h"
#include "Blob.h"

using namespace std;
using namespace cv;

/*
 * Takes a vector of Blobs as an argument, and fills it if blobs were found.
 * the boolean returned indicates wether or not Blobs were found
 */
bool detectObjects(vector<Blob> &blobs, Mat binaryImage, int colour);

void createTrackbars(HSVbounds &bounds, string winName);

void imgProcess(HSVbounds hsvBounds, Mat &hsv, Mat &filtered);

//void momentsOfOrder1(const cv::Mat &img, int *moments); TODO: implement a method returning only the 1st order moments

bool filterBoard(HSVbounds hsvBounds, Mat binaryImage, RotatedRect &board);

bool isInROI(Point p, Point2f roi[]);

double computeProduct(Point p, Point2f a, Point2f b);

#endif //COUVABOT_OBJDETECT_H
