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

using namespace std;

GenericCam *cam;
HSVbounds hsvBoundsGreen, hsvBoundsRed, whiteBoardBounds;
mvmtCtrl::mvmtController *mvCtrl;
Trajectory trajectory;
SPICom *spiCom;
HeartBeat *heartBeat;
Mat img, hsv, filtered;
vector<Blob> redBlobs, greenBlobs, processedRedBlobs, processedGreenBlobs;

pthread_mutex_t mutexBlobs, mutexCount;
bool endImgProc, loopStart;

unsigned int acquiredFrames, processedFrames;
unsigned startTime;

int checkTime();

int main(int argc, char **argv) {

    hsvBoundsRed.hMin = 0;
    hsvBoundsRed.hMax = 11;
    hsvBoundsRed.sMin = 119;
    hsvBoundsRed.sMax = 166;
    hsvBoundsRed.vMin = 144;
    hsvBoundsRed.vMax = 192;

    hsvBoundsGreen.hMin = 0;
    hsvBoundsGreen.hMax = 0;
    hsvBoundsGreen.sMin = 0;
    hsvBoundsGreen.sMax = 0;
    hsvBoundsGreen.vMin = 0;
    hsvBoundsGreen.vMax = 0;

    double vBat;

    if (argc > 1) {
        vBat = strtod(argv[1], NULL);
        cout << "vBat :" << vBat << " [V]\n";
    } else {
        cerr << "You need to call this program with the vbat value!\n";
        return 1;
    }

    // Initialize SPI communications
    if (RPI) {
        try {
            //first clock divider for Arduino, second for 2nd SPI peripheral
            spiCom = new SPICom(BCM2835_SPI_CLOCK_DIVIDER_128, BCM2835_SPI_CLOCK_DIVIDER_65536);
        } catch (string exception) {
            cerr << "Caught exception : " << exception << "\n";
        }
    }

    initialiseEpoch();

    acquiredFrames = 0;
    processedFrames = 0;

    mvCtrl = new mvmtCtrl::mvmtController(spiCom, vBat);
    mvCtrl->gearsCommand({0, 0});

    heartBeat = new HeartBeat(spiCom);

    // initialize the camera
    initCam();
    usleep(500000);

    mvCtrl->gearsCommand({.9, 1.});
    usleep(60000000);


    int rc = 0;
    pthread_t threads[3];
    rc = pthread_create(&threads[0], NULL, imgProc, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    } else
        cout << "imgProcessing thread launched\n";
    usleep(500000); // wait 500 [ms] to make sure the imgProcessing thread has stopped

    loopStart = true;
    rc = pthread_create(&threads[1], NULL, loop, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    } else
        cout << "loop thread launched\n";
    usleep(500000); // wait 500 [ms] to sure the loop thread has stopped

    int tmp = 0;
    scanf("%d", &tmp);
    cout << "End of program after " << millis() - startTime << " [ms]\n";
    cout << "number of processed frames n: " << acquiredFrames << "\n";
    endImgProc = true;

    pthread_cancel(threads[0]); // End the image processing thread
    pthread_cancel(threads[1]); // End the loop thread

    mvCtrl->gearsCommand({0, 0});


    if (RPI) {
        bcm2835_spi_end();
        bcm2835_close();
    }

    return 0;
}

void *loop(void *threadArgs) {

    int const middleX = FRAME_WIDTH / 2;

    while (heartBeat->start() != 1){
        // Waiting the start signal
    }

    cout << "Start signal received !\n";
    startTime = millis();

    gearsPower gearsSpeeds({0,0});

    unsigned int acqFrames = 0;

    int const bufferSize = 10;
    Blob blobsBuffer[bufferSize];
    for (int i = 0; i < bufferSize; ++i) {
        blobsBuffer[i] = Blob(0, 0, -1, 0);
    }

    while (checkTime() == 0) {

        pthread_mutex_lock(&mutexCount);
        acqFrames = acquiredFrames;
        pthread_mutex_unlock(&mutexCount);

        if (acqFrames > processedFrames) {    // New processed image

            processedFrames = acqFrames;

            // Lock variables to prevent (and ensure) that the blobs aren't being modified by another thread
            pthread_mutex_lock(&mutexBlobs);

            Blob *maxG, *maxR, *target;

            if (processedGreenBlobs.size() > 0) {
                maxG = &processedGreenBlobs.at(0);  // Get blob with biggest area
            } else {
                maxG = new Blob();
                cout << "No green blobs\n";
            }
            if (processedRedBlobs.size() > 0) {
                maxR = &processedRedBlobs.at(0);  // Get blob with biggest area
            } else {
                maxR = new Blob();
                cout << "No red blobs\n";
            }

            pthread_mutex_unlock(&mutexBlobs);

            target = (maxG->getArea() > maxR->getArea()) ? maxG : maxR;

            for (int i = 1; i < bufferSize; ++i) {
                blobsBuffer[i] = blobsBuffer[i - 1];
            }
            blobsBuffer[0] = *target;

            int i = 0;
            bool targetFound = false;
            while (!targetFound && i < bufferSize) {
                if (blobsBuffer[i].getArea() >= 0)
                    targetFound = true;
                else
                    i++;
            }

            if (!targetFound) {
                //mvCtrl->gearsCommand({0, 0});
                //cout << "No target found\n";
                continue;
            }

            target = &blobsBuffer[i];


            cout << "Target found. posX: " << target->getPosX() << " posY: " << target->getPosY() <<
            " ,aire " << target->getArea() << " et couleur " << target->getColour() << "\n";

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
                if ((target->getPosX() - middleX) < -2) {
                    mvCtrl->prepareLeft();
                    cout << "Prepared left container\n";
                }
                else {
                    mvCtrl->prepareRight();
                    cout << "Prepared right container\n";
                }
            } else if (dTarget <= 25) {
                speed = 0.0;
                mvCtrl->catchPuck();
            }


            gearsSpeeds = getDiffParams(target->getPosX(), target->getPosY(), speed);

            cout << "Number of processed frames : " << processedFrames << "\n";
        }



    }

    cout << "Loop thread cleanly closed\n";

    pthread_exit(NULL);
}

int checkTime() {
    return (millis() - startTime) > 60 * 1000;
}

void *imgProc(void *threadArgs) {

    if (CALIB) {
        //createTrackbars(hsvBoundsGreen, "Green Trackbars");
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
            //imshow("camera", img);
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

        // lock & unlock the counter
        pthread_mutex_lock(&mutexCount);
        acquiredFrames++;
        pthread_mutex_unlock(&mutexCount);

        if (CALIB)
            waitKey(50);
    }

    cout << "Image processing thread cleanly closed\n";

    pthread_exit(NULL);
}

void capBlobs() {

    imgProcess(hsvBoundsRed, hsv, filtered);
    detectObjects(redBlobs, filtered, RED);
    if (CALIB)
        imshow("Red filtered", filtered);

    /*
    imgProcess(hsvBoundsGreen, hsv, filtered);
    detectObjects(greenBlobs, filtered, GREEN);
    if (CALIB)
        imshow("Green filtered", filtered);
    */
}

void capImage() {
    cam->read(img);
    cvtColor(img, hsv, COLOR_BGR2HSV);
}

void initCam() {
    if (RPI)
        cam = new WebCam();
    else
        cam = new WebCam();
}
