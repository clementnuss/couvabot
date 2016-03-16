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
#include "navigation/differentialDrive.h"

GenericCam *cam;
HSVbounds hsvBoundsGreen, hsvBoundsRed, whiteBoardBounds;
mvmtCtrl::mvmtController *mvCtrl;
Trajectory trajectory;
SPICom *spiCom;
HeartBeat *heartBeat;
Mat img, hsv, filtered;
vector<Blob> redBlobs, greenBlobs, processedRedBlobs, processedGreenBlobs;

pthread_mutex_t mutexBlobs;
bool endImgProc;

unsigned int n;
unsigned startTime;

int checkTime();

int main(int argc, char **argv) {

    double vBat;

    if (argc > 1) {
        vBat = strtod(argv[1], NULL);
        cout << "vBat :" << vBat << " [V]\n";
    } else {
        cerr << "You need to call this program with the vbat value!\n";
        return 1;
    }

    if (!initCam()) {
        cerr << "camera initialization error !!!";
        return 1;
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

    startTime = millis();
    n = 0;


    int rc = 0;
    pthread_t threads[3];
    rc = pthread_create(&threads[0], NULL, imgProc, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    } else
        cout << "imgProcessing thread launched\n";

    rc = pthread_create(&threads[1], NULL, loop, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    } else
        cout << "loop thread launched\n";


    int tmp = 0;
    scanf("%d", &tmp);
    cout << "End of program after " << millis() - startTime << " [ms]\n";
    cout << "number of processed frames n: " << n << "\n";
    endImgProc = true;

    pthread_cancel(threads[0]); // End the image processing thread
    usleep(1000000);

    rc = pthread_create(&threads[2], NULL, imgProc, NULL);
    scanf("%d", &tmp);


    mvCtrl->arduiCommand({0, 0});


    if (RPI) {
        bcm2835_spi_end();
        bcm2835_close();
    }

    return 0;
}

void *loop(void *threadArgs) {
    int procN = 0;  // number of processed frames

    while (checkTime() != 1) {

        if (n > procN) {    // New processed image

            // Lock variables to prevent (and ensure) that the blobs aren't being modified by another thread
            pthread_mutex_lock(&mutexBlobs);

            Blob *maxG, *maxR, *target;

            if (processedGreenBlobs.size() > 0) {
                maxG = &processedGreenBlobs.at(0);  // Get blob with biggest area
            } else {
                maxG = new Blob(0, 0, -1, GREEN);
            }
            if (processedRedBlobs.size() > 0) {
                maxR = &processedRedBlobs.at(0);  // Get blob with biggest area
            } else {
                maxR = new Blob(0, 0, -1, RED);

            }

            target = (maxG->getArea() > maxR->getArea()) ? maxG : maxR;
            double dTarget = project(target->getPosX(), target->getPosY());
            double speed = 0;
            if (dTarget > 80) {
                speed = 1.0;
            } else if (dTarget > 60) {
                speed = 0.7;
            } else if (dTarget > 40) {
                speed = 0.35;
            } else if (dTarget > 30) {
                speed = 0.1;
            } else if (dTarget <= 25) {
                speed = 0.0;
            }

            mvCtrl->arduiCommand(getParams(target->getPosX(), target->getPosY(), speed));
            procN++;

            pthread_mutex_unlock(&mutexBlobs);
        }


    }

    if (heartBeat->pingArduino()) {
        cout << "Arduino feedback received\n";
    }

    pthread_exit(NULL);
}

int checkTime() {
    return (millis() - startTime) < 105000;
}

void *imgProc(void *threadArgs) {

    if (CALIB) {
        createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
        //createTrackbars(whiteBoardBounds, "WhiteBoard bounds");
    }

    while (!endImgProc) {

        capImage();

        // Code to detect the board!
/*
        Mat whiteBoardFiltered;
        imgProcess(whiteBoardBounds, hsv, whiteBoardFiltered);
        imshow("whiteBoard filtered", whiteBoardFiltered);

        RotatedRect board;
        filterBoard(whiteBoardBounds, whiteBoardFiltered, board);

        Point2f vertices;
        board.points(&vertices);

        Mat mask = Mat(hsv.size(), CV_8U, Scalar(0));
        for (int i = 0; i < img.rows; ++i) {
            for (int j = 0; j < img.cols; ++j) {
                Point p = Point(j, i);   // pay attention to the cordination
                if (isInROI(p, &vertices)) {
                    mask.at<uchar>(i, j) = 255;
                }
            }
        }

        Scalar color = Scalar(155, 165, 23);
        Point2f rect_points[4];
        board.points(rect_points);
        for (int j = 0; j < 4; j++)
            line(img, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
*/

        if (CALIB) {
            imshow("camera", img);
            imshow("HSV image", hsv);
        }
//

        capBlobs();

        sort(redBlobs.begin(), redBlobs.end(), compBlobs);
        sort(greenBlobs.begin(), greenBlobs.end(), compBlobs);

        // Lock variables to prevent (and ensure) that the blobs aren't being modified by another thread
        pthread_mutex_lock(&mutexBlobs);

        processedRedBlobs = redBlobs;
        processedGreenBlobs = greenBlobs;

        // unlock variables
        pthread_mutex_unlock(&mutexBlobs);

        n++;

        if (CALIB)
            waitKey(30);
    }

    cout << "Loop thread cleanly closed\n";

    pthread_exit(NULL);
}

void capBlobs() {

    imgProcess(hsvBoundsRed, hsv, filtered);
    detectObjects(redBlobs, filtered, RED);
    if (CALIB)
        imshow("Red filtered", filtered);


    imgProcess(hsvBoundsGreen, hsv, filtered);
    detectObjects(greenBlobs, filtered, GREEN);
    if (CALIB)
        imshow("Green filtered", filtered);
}

void capImage() {
    cam->read(img);
    cvtColor(img, hsv, COLOR_BGR2HSV);
}

bool initCam() {
    if (RPI)
        cam = new WebCam();
    else
        cam = new WebCam();
    return true;
}
