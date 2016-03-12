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
#include "physIface/HeartBeat.h"
#include "navigation/Trajectory.h"

int CAMERA_ANGLE = 0;
GenericCam *cam;
HSVbounds hsvBoundsGreen, hsvBoundsRed;
mvmtCtrl::mvmtController *mvCtrl;
Trajectory trajectory;
SPICom *spiCom;
HeartBeat *heartBeat;

unsigned startTime = millis();

bool initCam() {
    if (RPI)
        cam = new RPiCam();
    else
        cam = new WebCam();
    return true;
}


int main(int argc, char **argv) {

    initialiseEpoch();

    Mat img, hsv, filtered;
    vector<Blob> redBlobs, greenBlobs;

    /*if (!initCam())
        cerr << "Camera initialization error !!!";*/


    if (CALIB) {
        createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
    }

    if (RPI) {
        try {
            //first clock divider for Arduino
            spiCom = new SPICom(BCM2835_SPI_CLOCK_DIVIDER_128, BCM2835_SPI_CLOCK_DIVIDER_65536);
        } catch (string exception) {
            cerr << "Caught exception : " << exception;
        }
    }

    mvCtrl = new mvmtCtrl::mvmtController(spiCom);
    mvCtrl->arduiCommand({0, 0});    // TODO: solve arduino initialization bug to avoid this command

    heartBeat = new HeartBeat(spiCom);

    while (heartBeat->start() != 0){


    }

    startTime = millis();


    int speedInt = 100;
    double speed = 1.0;
    int c = 0;
    bool left, right;


    trajectory = Trajectory();
    trajectory.setParams(0.15, 0.8, 0.3);
    trajectory.setWheelsPower(0.8);

    while (trajectory.update() == 0) {
        usleep(50);
    }

    mvCtrl->arduiCommand(trajectory.getWheelsPower());

    while (trajectory.update() == 0) {
        usleep(50);
    }

    cout << "end of traj" << "\n";

    mvCtrl->arduiCommand({0, 0});

    cout << "Success!\n";


#if CALIB
    namedWindow("Speeds", 0);
    createTrackbar("speedPer", "Speeds", &speedInt, 100, nullptr);

    mvmtCtrl::gearsPower powers;
    while (1) {

        switch ((c = waitKey(200))) {
            case KEY_UP:
                cout << endl << "Up" << endl;//key up
                speedPer = std::min(speedPer + 0.1, 1.0);
                powers.pL=speedPer;
                powers.pR=speedPer;
                mvCtrl->arduiCommand(powers);
                break;
            case KEY_DOWN:
                cout << endl << "Down" << endl;   // key down
                speedPer = std::max(speedPer - 0.1, -1.0);
                powers.pL=speedPer;
                powers.pR=speedPer;
                mvCtrl->arduiCommand(powers);
                break;
            case KEY_LEFT:
                if (left) {
                    powers.pL=speedPer;
                    powers.pR=speedPer;
                    mvCtrl->arduiCommand(powers);
                    left = false;
                    break;
                }
                else {
                    cout << endl << "Left" << endl;  // key left
                    powers.pL=speedPer - 0.3f;
                    powers.pR=speedPer;
                    mvCtrl->arduiCommand(powers);
                    left = true;
                    break;
                }
            case KEY_RIGHT:
                if (right) {
                    powers.pL=speedPer - 0.3f;
                    powers.pR=speedPer;
                    mvCtrl->arduiCommand(powers);
                    right = false;
                    break;
                }
                else {
                    cout << endl << "Right" << endl;  // key right
                    powers.pL=speedPer;
                    powers.pR=speedPer - 0.3f;
                    mvCtrl->arduiCommand(powers);
                    right = true;
                    break;
                }
            default:
                cout << endl << "char : " << c << endl;  // not arrow
                break;
        }

        speedInt = (int) (speedPer * 100);

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

int loop() {

    if (trajectory.update()) {
        mvCtrl->arduiCommand(trajectory.getWheelsPower());
    }


}

int checkTime() {
    return (millis() > 105000);
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
