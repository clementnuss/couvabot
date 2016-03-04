//
// Created by Clément Nussbaumer on 10.02.16.
//
//
//

#include "main.h"
#include "src/detectObjects.h"
#include "src/SPICom.h"

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

    Mat          img, hsv, filtered;
    vector<Blob> redBlobs, greenBlobs;
    uint8_t      readData, sendData;

    if (!initCam())
        cerr << "Camera initialization error !!!";


    if (CALIB) {
        createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
    }

    int frameCnt = 0;

    while (1) {


        cam->read(img);
        cvtColor(img, hsv, COLOR_BGR2HSV);

        cout << "Frame # " << frameCnt++ << "\n\n\n";

        capBlobs(hsv, filtered, redBlobs, greenBlobs);


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

        if (redBlobs.size() >= greenBlobs.size()) {


        }


        //show frames
        if (CALIB) {
            imshow("camera", img);
            imshow("HSV image", hsv);
        }
        if (waitKey(500) == 27)
            break;

    }


    if (RPI) {
        bcm2835_spi_end();
        bcm2835_close();
    }
    cout << "End of program!";


    return 0;
}

void capBlobs(Mat &hsv, Mat &filtered, vector<Blob> &redBlobs, vector<Blob> &greenBlobs) {

    imgProc(hsvBoundsRed, hsv, filtered);
    detectObjects(redBlobs, filtered, RED);
    if (CALIB)
        imshow("Red filtered", filtered);


    imgProc(hsvBoundsGreen, hsv, filtered);
    detectObjects(greenBlobs, filtered, GREEN);
    if (CALIB)
        imshow("Green filtered", filtered);
}