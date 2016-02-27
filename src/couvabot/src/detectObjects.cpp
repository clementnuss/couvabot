//
// Created by clement on 26.02.2016.
//

#include "detectObjects.h"

//Minimum/maximum detection settings
const int MIN_AREA = 20 * 20;
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
            //TODO: optimize moments method to only compute m00, m01 and m10
            Moments moment(moments((Mat) contours[i]));

            int area = (int) moment.m00;

            if (area > MIN_AREA && area < MAX_AREA) {
                int x = (int) (moment.m10 / area);
                int y = (int) (moment.m01 / area);
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
