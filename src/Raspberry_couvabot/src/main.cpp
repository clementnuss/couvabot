//
// Created by Clément Nussbaumer on 10.02.16.
//
//
//

#include <unistd.h>

#include "main.h"
#include "imageProcessing/detectObjects.h"
#include "physIface/mvmtController.h"
#include "physIface/localTime.h"
#include "navigation/Trajectory.h"

int CAMERA_ANGLE = 0;
GenericCam *cam;
HSVbounds hsvBoundsGreen, hsvBoundsRed;
mvmtCtrl::mvmtController *mvCtrl;

bool initCam() {
    if (RPI)
        cam = new RPiCam();
    else
        cam = new WebCam();
    return true;
}


int main(int argc, char **argv) {

    initialiseEpoch();

    cout << micros() << endl;
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

    for (int i = 0; i < 10000; ++i) {
        c++;
    }
    cout << "il faut " << micros() << " us pour compter jusqu'a 10000\n";

    Trajectory trajectory(5, 13, 8);

    trajectory.getWheelsPower(1);

    while (trajectory.updateAngle() == 0) {
        cout << "Waiting a few us";
        usleep(500);
    }

    int time = millis();

    int cnt = 0;

    while (cnt <= 10) {
        int diff = 0;
        do {
            diff = millis() - time;
        } while (diff < 1000);
        cout << cnt++ << "\n";
    }

    cout << "Success!\n";


#if CALIB
    namedWindow("Speeds", 0);
    createTrackbar("speed", "Speeds", &speedInt, 100, nullptr);

    mvmtCtrl::gearsPower powers;
    while (1) {

        switch ((c = waitKey(200))) {
            case KEY_UP:
                cout << endl << "Up" << endl;//key up
                speed = std::min(speed + 0.1, 1.0);
                powers.pL=speed;
                powers.pR=speed;
                mvCtrl->arduiCommand(powers);
                break;
            case KEY_DOWN:
                cout << endl << "Down" << endl;   // key down
                speed = std::max(speed - 0.1, -1.0);
                powers.pL=speed;
                powers.pR=speed;
                mvCtrl->arduiCommand(powers);
                break;
            case KEY_LEFT:
                if (left) {
                    powers.pL=speed;
                    powers.pR=speed;
                    mvCtrl->arduiCommand(powers);
                    left = false;
                    break;
                }
                else {
                    cout << endl << "Left" << endl;  // key left
                    powers.pL=speed - 0.3f;
                    powers.pR=speed;
                    mvCtrl->arduiCommand(powers);
                    left = true;
                    break;
                }
            case KEY_RIGHT:
                if (right) {
                    powers.pL=speed - 0.3f;
                    powers.pR=speed;
                    mvCtrl->arduiCommand(powers);
                    right = false;
                    break;
                }
                else {
                    cout << endl << "Right" << endl;  // key right
                    powers.pL=speed;
                    powers.pR=speed - 0.3f;
                    mvCtrl->arduiCommand(powers);
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

#endif

    if (RPI) {
        bcm2835_spi_end();

        bcm2835_close();

    }
    cout << "End of program!";


    return 0;
}

void loop() {


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
