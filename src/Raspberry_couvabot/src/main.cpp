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
HSVbounds hsvBoundsGreen, hsvBoundsRed, whiteBoardBounds;
mvmtCtrl::mvmtController *mvCtrl;
Trajectory trajectory;
SPICom *spiCom;
HeartBeat *heartBeat;
Mat img, hsv, filtered;
vector<Blob> redBlobs, greenBlobs;

unsigned int n;

bool imgReady = false, blobsReady = false;

unsigned startTime;

int main(int argc, char **argv) {

    double vBat;

    /*
    if (argc > 1) {
        vBat = strtod(argv[2], NULL);
        cout << "vBat :" << vBat << " [V]\n";
    } else {
        cerr << "You need to call this program with the vbat value!\n";
        return 1;
    }*/

    if (!initCam()) {
        cerr << "camera initialization error !!!";
        return 1;
    }


    if (CALIB) {
        createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
        createTrackbars(whiteBoardBounds, "WhiteBoard bounds");
    }

    if (!RPI) {
        try {
            //first clock divider for Arduino, second for 2nd SPI peripheral
            spiCom = new SPICom(BCM2835_SPI_CLOCK_DIVIDER_128, BCM2835_SPI_CLOCK_DIVIDER_65536);
        } catch (string exception) {
            cerr << "Caught exception : " << exception;
        }
    }

    initialiseEpoch();

    /*
    mvCtrl = new mvmtCtrl::mvmtController(spiCom, vBat);
    mvCtrl->arduiCommand({0, 0});    // TODO: solve arduino initialization bug to avoid this command

    heartBeat = new HeartBeat(spiCom);

    /*
     while (heartBeat->start() != 0) {


    }
    */

    startTime = millis();

/*
    trajectory = Trajectory();
    trajectory.setParams(0.15, 0.8, 0.3);
    trajectory.setWheelsPower(0.8);

    cout << "end of trajectory" << "\n";
*/
    //mvCtrl->arduiCommand({0, 0});

    cout << "Success!\n";
    cout << "End of program! after " << millis() - startTime << " [ms]\n";

    int rc;

    pthread_t threads[5];
    rc = pthread_create(&threads[0], NULL, imgProc, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    }

    scanf("%d", &rc);

    cout << "n: " << n << "\n";
    cout << "FPS: " << n / 10 << "\n";


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

void *imgProc(void *threadArgs) {

    while (1) {

        capImage();

        Mat whiteBoardFiltered;
        imgProcess(whiteBoardBounds, hsv, whiteBoardFiltered);
        Rect board;
        filterBoard(whiteBoardBounds, whiteBoardFiltered, board);

        Scalar color = Scalar(120, 120, 0);
        rectangle(img, board.tl(), board.br(),color, 2, 8, 0);

        blobsReady = false;

        capBlobs();

        if (CALIB) {
            imshow("camera", img);
            imshow("HSV image", hsv);
        }
//
        sort(redBlobs.begin(), redBlobs.end(), compBlobs);
        sort(greenBlobs.begin(), greenBlobs.end(), compBlobs);

        blobsReady = true;
        n++;

        waitKey(30);
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

}

void capBlobs() {

    blobsReady = false;

    imgProcess(hsvBoundsRed, hsv, filtered);
    detectObjects(redBlobs, filtered, RED);
    //if (CALIB)
        imshow("Red filtered", filtered);


    imgProcess(hsvBoundsGreen, hsv, filtered);
    detectObjects(greenBlobs, filtered, GREEN);
    //if (CALIB)
        imshow("Green filtered", filtered);

    blobsReady = true;
}

void capImage() {
    cam->read(img);
    cvtColor(img, hsv, COLOR_BGR2HSV);
}

bool initCam() {
    if (RPI)
        cam = new RPiCam();
    else
        cam = new WebCam();
    return true;
}