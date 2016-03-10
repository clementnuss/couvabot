//
// Created by Clément Nussbaumer on 10.02.16.
//
//
//

#include <unistd.h>
#include "main.h"
#include "imageProcessing/detectObjects.h"
#include "physIface/SPICom.h"
#include "physIface/mvmtController.h"

int CAMERA_ANGLE = 0;
GenericCam *cam;
HSVbounds hsvBoundsGreen, hsvBoundsRed;
mvmtCtrl::mvmtController *mvCtrl;

bool initCam() {
    if (RPI)
        cam = new RPiCam();
    else
        cam = new WebCam();
}


int main(int argc, char **argv) {

    Mat img, hsv, filtered;
    vector<Blob> redBlobs, greenBlobs;

    /*if (!initCam())
        cerr << "Camera initialization error !!!";*/


    if (CALIB) {
        createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
    }

    mvCtrl = new mvmtCtrl::mvmtController();

    int speedInt = 100;
    double speed = 1.0;
    int c = 0;
    bool left, right;


    namedWindow("Speeds", 0);
    createTrackbar("speed", "Speeds", &speedInt, 100, nullptr);

    while (1) {

        switch ((c = waitKey(200))) {
            case KEY_UP:
                cout << endl << "Up" << endl;//key up
                speed = std::min(speed + 0.1, 1.0);
                mvCtrl->arduiCommand(speed, speed);
                break;
            case KEY_DOWN:
                cout << endl << "Down" << endl;   // key down
                speed = std::max(speed - 0.1, -1.0);

                mvCtrl->arduiCommand(speed, speed);
                break;
            case KEY_LEFT:
                if (left) {
                    mvCtrl->arduiCommand(speed, speed);
                    left = false;
                    break;
                }
                else {
                    cout << endl << "Left" << endl;  // key left
                    mvCtrl->arduiCommand(speed - 0.3f, speed);
                    left = true;
                    break;
                }
            case KEY_RIGHT:
                if (right) {
                    mvCtrl->arduiCommand(speed, speed);
                    right = false;
                    break;
                }
                else {
                    cout << endl << "Right" << endl;  // key right
                    mvCtrl->arduiCommand(speed, speed - 0.3f);
                    right = true;
                    break;
                }
            default:
                cout << endl << "char : " << c << endl;  // not arrow
                break;
        }

        speedInt = (int) (speed * 100);

        usleep(100000);
    }

    while (1) {


        cam->read(img);
        cvtColor(img, hsv, COLOR_BGR2HSV);

        // cout << "Frame # " << frameCnt++ << "\n\n\n";

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
