//
// Created by clement on 26.02.2016.
//

#ifndef COUVABOT_OBJDETECT_H
#define COUVABOT_OBJDETECT_H

#include "../main.h"


/*
 * Takes a vector of Blobs as an argument, and fills it if blobs were found.
 * the boolean returned indicates wether or not Blobs were found
 */
bool detectObjects(vector<Blob> &blobs, Mat binaryImage, int colour);
void createTrackbars(HSVbounds &bounds, string winName);
void imgProc(HSVbounds hsvBounds, Mat &hsv, Mat &filtered);

#endif //COUVABOT_OBJDETECT_H