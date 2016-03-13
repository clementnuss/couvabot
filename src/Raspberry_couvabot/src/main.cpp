//
// Created by Clément Nussbaumer on 10.02.16.
//
//
//

#include <unistd.h>
#include <pthread.h>

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
Mat img, hsv, filtered;
vector<Blob> redBlobs, greenBlobs;

bool imgReady = false, blobsReady = false;

unsigned startTime;

int main(int argc, char **argv) {

    double vBat;
    if (argc > 1) {
        vBat = strtod(argv[2], NULL);
    } else {
        cerr << "You need to call this program with the vbat value!\n";
        return 1;
    }

    if (!initCam()){
        cerr << "Camera initialization error !!!";
        return 1;
    }


    if (CALIB) {
        createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
    }

    if (RPI) {
        try {
            //first clock divider for Arduino, second for 2nd SPI peripheral
            spiCom = new SPICom(BCM2835_SPI_CLOCK_DIVIDER_128, BCM2835_SPI_CLOCK_DIVIDER_65536);
        } catch (string exception) {
            cerr << "Caught exception : " << exception;
        }
    }

    initialiseEpoch();

    mvCtrl = new mvmtCtrl::mvmtController(spiCom, vBat);
    mvCtrl->arduiCommand({0, 0});    // TODO: solve arduino initialization bug to avoid this command

    heartBeat = new HeartBeat(spiCom);

    /*
     while (heartBeat->start() != 0) {


    }
    */

    startTime = millis();


    trajectory = Trajectory();
    trajectory.setParams(0.15, 0.8, 0.3);
    trajectory.setWheelsPower(0.8);


    cout << "end of trajectory" << "\n";

    mvCtrl->arduiCommand({0, 0});

    cout << "Success!\n";

    cout << "End of program! after " << millis() - startTime << " [ms].";

    while(1){

        usleep(50000);

        capImage();

        //show frames
        if (CALIB) {
            imshow("camera", img);
            imshow("HSV image", hsv);
        }

    }


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

    sort(redBlobs.begin(), redBlobs.end(), compBlobs);
    sort(greenBlobs.begin(), greenBlobs.end(), compBlobs);


    if (RPI) {
        bcm2835_spi_end();
        bcm2835_close();
    }


    return 0;
}

int loop() {

    if (trajectory.update()) {
        mvCtrl->arduiCommand(trajectory.getWheelsPower());
    }

    return 0;
}

void *capBlobs() {

    if (!imgReady) {
        pthread_exit((void *) 1);
    }

    blobsReady = false;

    imgProc(hsvBoundsRed, hsv, filtered);
    detectObjects(redBlobs, filtered, RED);
    if (CALIB)
        imshow("Red filtered", filtered);


    imgProc(hsvBoundsGreen, hsv, filtered);
    detectObjects(greenBlobs, filtered, GREEN);
    if (CALIB)
        imshow("Green filtered", filtered);

    blobsReady = true;

    pthread_exit(NULL);
    return nullptr;
}

void *capImage() {

    imgReady = false;

    cam->read(img);
    cvtColor(img, hsv, COLOR_BGR2HSV);

    imgReady = true;

    pthread_exit(NULL);
    return nullptr;
}

bool initCam() {
    if (RPI)
        cam = new RPiCam();
    else
        cam = new WebCam();
    return true;
}