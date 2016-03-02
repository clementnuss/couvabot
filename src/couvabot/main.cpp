//
// Created by Clément Nussbaumer on 10.02.16.
//
//
//

#include "main.h"
#include "src/detectObjects.h"

int        CAMERA_ANGLE = 0;
GenericCam *cam;
HSVbounds  hsvBoundsGreen, hsvBoundsRed;

bool initCam() {
    if (RPI)
        cam = new RPiCam();
    else
        cam = new WebCam();
}


int main(int argc, char **argv) {

    if (!initCam())
        cerr << "Camera initialization error !!!";

    if (CALIB) {
        createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
    }

    Mat img, hsv, filtered;

    vector<Blob> redBlobs, greenBlobs;
    int          frameCnt = 0;

    while (1) {

        cam->read(img);
        cvtColor(img, hsv, COLOR_BGR2HSV);

        imgProc(hsvBoundsRed, hsv, filtered);
        detectObjects(redBlobs, filtered, RED);
        if (CALIB)
            imshow("Red filtered", filtered);


        imgProc(hsvBoundsGreen, hsv, filtered);
        detectObjects(greenBlobs, filtered, GREEN);
        if (CALIB)
            imshow("Green filtered", filtered);

        cout << "Frame # " << frameCnt++ << "\n\n\n";

        /*if (redBlobs.size() != 0) {
            for (int i = 0; i < redBlobs.size(); i++) {
                cout << "Blob x :  " << redBlobs[i].getPosX()
                << " y : " << redBlobs[i].getPosY() <<
                " ; aire : " << redBlobs[i].getArea() << "\t";
            }
        }

        if (greenBlobs.size() != 0) {
            for (int i = 0; i < greenBlobs.size(); i++) {
                cout << "Blob x :  " << greenBlobs[i].getPosX()
                << " y : " << greenBlobs[i].getPosY() <<
                " ; aire : " << greenBlobs[i].getArea() << "\t";
            }
        }*/



        //show frames
        if (CALIB) {
            imshow("camera", img);
            imshow("HSV image", hsv);
        }
        if (waitKey(500) == 27)
            break;

    }

    return 0;
}