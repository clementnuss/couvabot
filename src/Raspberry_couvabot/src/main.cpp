//
// Created by Clément Nussbaumer on 10.02.16.
//
//
//

#include <unistd.h>
#include <pthread.h>

#include "main.h"
#include "imageProcessing/detectObjects.h"
#include "physIface/arduinoComm.h"
#include "physIface/localTime.h"
#include "navigation/differentialDrive.h"
#include "navigation/braitenberg.h"

using namespace std;

GenericCam *cam;
HSVbounds hsvBoundsGreen, hsvBoundsRed, whiteBoardBounds, adversaryBaseBounds, homeBaseBounds;
ardCom::arduinoComm *arduinoComm;
SPICom *spiCom;
Mat img, hsv, filtered;
vector<Blob> redBlobs, greenBlobs, processedRedBlobs, processedGreenBlobs, baseBlobs, cachedBaseBlobs;

gearsPower gearsSpeeds({0, 0});

pthread_mutex_t mutexBlobs, mutexCount, mutexFindBase;
bool endImgProc, disableBraiten;
int findBase;

unsigned int acquiredFrames, processedFrames, braitenTime;
unsigned startTime;
int leftContainer[2], rightContainer[2];

int main(int argc, char **argv) {
/*
    // With light
    hsvBoundsRed.hMin = 0;
    hsvBoundsRed.hMax = 8;
    hsvBoundsRed.sMin = 115;
    hsvBoundsRed.sMax = 206;
    hsvBoundsRed.vMin = 117;
    hsvBoundsRed.vMax = 182;

    hsvBoundsGreen.hMin = 58;
    hsvBoundsGreen.hMax = 79;
    hsvBoundsGreen.sMin = 24;
    hsvBoundsGreen.sMax = 100;
    hsvBoundsGreen.vMin = 43;
    hsvBoundsGreen.vMax = 95;
*/

    hsvBoundsRed.hMin = 0;
    hsvBoundsRed.hMax = 21;
    hsvBoundsRed.sMin = 149;
    hsvBoundsRed.sMax = 224;
    hsvBoundsRed.vMin = 106;
    hsvBoundsRed.vMax = 217;

    adversaryBaseBounds.hMin = 0;
    adversaryBaseBounds.hMax = 0;
    adversaryBaseBounds.sMin = 0;
    adversaryBaseBounds.sMax = 0;
    adversaryBaseBounds.vMin = 0;
    adversaryBaseBounds.vMax = 0;

    homeBaseBounds.hMin = 0;
    homeBaseBounds.hMax = 0;
    homeBaseBounds.sMin = 0;
    homeBaseBounds.sMax = 0;
    homeBaseBounds.vMin = 0;
    homeBaseBounds.vMax = 0;

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

    arduinoComm = new ardCom::arduinoComm(spiCom, vBat);
    arduinoComm->gearsCommand({0, 0});

    ardCom::sensorsData sensors;
    startTime = millis();

    // initialize the camera
    initCam();
    usleep(500000);

    int rc = 0;
    pthread_t threads[3];
    rc = pthread_create(&threads[0], NULL, imgProc, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    } else
        cout << "imgProcessing thread launched\n";
    usleep(200000); // wait 200 [ms] to make sure the imgProcessing thread has stopped

    rc = pthread_create(&threads[1], NULL, loop, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    } else
        cout << "loop thread launched\n";
    usleep(200000); // wait 200 [ms] to sure the loop thread has stopped

    int tmp = 0;
    scanf("%d", &tmp);
    cout << "End of program after " << millis() - startTime << " [ms]\n";
    cout << "number of processed frames n: " << acquiredFrames << "\n";
    endImgProc = true;

    pthread_cancel(threads[0]); // End the image processing thread
    pthread_cancel(threads[1]); // End the loop thread

    arduinoComm->gearsCommand({0, 0});


    if (RPI) {
        bcm2835_spi_end();
        bcm2835_close();
    }

    return 0;
}

void *loop(void *threadArgs) {

    int const middleX = FRAME_WIDTH / 2;

    /*
    while (heartBeat->start() != 1){
        // Waiting the start signal
    }
    */
    cout << "Start signal received !\n";
    startTime = millis();

    ardCom::sensorsData sensors;
    braitenTime = millis();

    unsigned int acqFrames = 0;
    int redPucks = 0, greenPucks = 0;

    int const bufferSize = 10;
    Blob blobsBuffer[bufferSize];
    for (int i = 0; i < bufferSize; ++i) {
        blobsBuffer[i] = Blob(0, 0, -1, 0);
    }

    while (checkEnd() == 0) {

        if (checkReturnTime()){
            pthread_mutex_lock(&mutexFindBase);
            findBase = HOME;
            pthread_mutex_unlock(&mutexFindBase);
        }

        pthread_mutex_lock(&mutexCount);
        acqFrames = acquiredFrames;
        pthread_mutex_unlock(&mutexCount);

        if (acqFrames > processedFrames) {    // New processed image
            processedFrames = acqFrames;

            if (findBase || redPucks == 1 && greenPucks == 1) {   // When there are 4 pucks, go to the base

                if (redPucks == 1 && greenPucks == 1) {
                    pthread_mutex_lock(&mutexFindBase);
                    findBase = HOME;
                    pthread_mutex_unlock(&mutexFindBase);

                    redPucks = 0;   // Reset the puck counters
                    greenPucks = 0;

                    // Reset the blobs buffer
                    for (int j = 0; j < bufferSize; ++j) {
                        blobsBuffer[j] = Blob(0, 0, -1, 0);
                    }
                } else {        // we are in the findBase state

                    // Lock variables to prevent (and ensure) that the blobs aren't being modified by another thread
                    pthread_mutex_lock(&mutexBlobs);

                    Blob *base;

                    if (processedGreenBlobs.size() > 0) {
                        base = &cachedBaseBlobs.at(0);  // Get blob with biggest area
                    } else {
                        base = new Blob();
                    }

                    pthread_mutex_unlock(&mutexBlobs);

                    for (int i = 1; i < bufferSize; ++i) {
                        blobsBuffer[i] = blobsBuffer[i - 1];
                    }
                    blobsBuffer[0] = *base;

                    int i = 0;
                    bool targetFound = false;
                    while (!targetFound && i < bufferSize) {
                        if (blobsBuffer[i].getArea() >= 0)
                            targetFound = true;
                        else
                            i++;
                    }

                    if (!targetFound) {
                        gearsSpeeds = {0.4, 0.4};
                        goto braiten;
                    }

                    base = &blobsBuffer[i];

                    double dTarget = project(base->getPosX(), base->getPosY());

                    cout << "Base found. posX: " << base->getPosX() << " posY: " << base->getPosY() <<
                    " ,aire " << base->getArea() << " et couleur " << base->getColour()
                    << " et distance " << dTarget << "\n";

                    double speed = 0;
                    if (dTarget > 80) {
                        speed = 1.0;
                    } else if (dTarget > 60) {
                        speed = 0.7;
                    } else if (dTarget > 45) {
                        speed = 0.5;
                    } else if (dTarget > 37) {
                        speed = 0.35;
                    } else if (dTarget <= 35) {
                        speed = 0.33;

                        gearsSpeeds = {speed, speed};

                        cout << "Approching base!\n";

                        unsigned int approachTime = millis();
                        while (millis() - approachTime < 1500 * 1000)    // Approach base for 1500 [ms]
                            // When reaching the base, we use braitenberg to (quite randomly) position ourselves
                            if (millis() - braitenTime < BRAITENBERG_RATE)
                                continue;
                            else {
                                braitenTime = millis();

                                sensors = arduinoComm->getData();
                                while (!sensors.valid) {
                                    cout << "Error while fetching sensors data ! Trying again in 50 [us].\n";
                                    usleep(50);
                                    sensors = arduinoComm->getData();
                                }

                                braiten(&gearsSpeeds, sensors);
                                arduinoComm->gearsCommand(gearsSpeeds);
                            }

                        arduinoComm->gearsCommand({0, 0});

                        arduinoComm->releasePuck();

                        // clear buffer
                        for (int j = 0; j < bufferSize; ++j) {
                            blobsBuffer[j] = Blob(0, 0, -1, 0);
                        }

                        usleep(1500 * 1000);

                        pthread_mutex_lock(&mutexFindBase);
                        findBase = 0;       // We will again be in the "normal" loop
                        pthread_mutex_unlock(&mutexFindBase);

                        continue;
                    }

                    // Base searching

                    gearsSpeeds = {speed, speed};
                    goto braiten;
                }
            } else {

                //  Puck tracking mode

                // Lock variables to prevent (and ensure) that the blobs aren't being modified by another thread
                pthread_mutex_lock(&mutexBlobs);

                Blob *maxG, *maxR, *target;

                if (processedGreenBlobs.size() > 0) {
                    maxG = &processedGreenBlobs.at(0);  // Get blob with biggest area
                } else {
                    maxG = new Blob();
                    if (DEBUG)
                        cout << "No green blobs\n";
                }
                if (processedRedBlobs.size() > 0) {
                    maxR = &processedRedBlobs.at(0);  // Get blob with biggest area
                } else {
                    maxR = new Blob();
                    if (DEBUG)
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
                    gearsSpeeds = {0.4, 0.4};
                    goto braiten;
                }

                target = &blobsBuffer[i];
                double dTarget = project(target->getPosX(), target->getPosY());

                cout << "Target found. posX: " << target->getPosX() << " posY: " << target->getPosY() <<
                " ,aire " << target->getArea() << " et couleur " << target->getColour() <<
                " et distance " << dTarget << "\n";

                double speed = 0;
                if (dTarget > 80) {
                    speed = 1.0;
                } else if (dTarget > 60) {
                    speed = 0.7;
                } else if (dTarget > 45) {
                    speed = 0.5;
                } else if (dTarget > 39) {
                    speed = 0.35;
                    disableBraiten = true;

                    if (target->getColour() == RED) {
                        if (target->getColour() == RED) {
                            if (!arduinoComm->prepareLeft()) {
                                usleep(50); // Wait 50 [us]
                                arduinoComm->prepareLeft();
                            }
                        }
                        cout << "Prepared left container for red puck\n";
                    } else {
                        if (target->getColour() == RED) {
                            if (!arduinoComm->prepareRight()) {
                                usleep(50); // Wait 50 [us]
                                arduinoComm->prepareRight();
                            }
                        }
                        cout << "Prepared right container for green puck\n";
                    }
                } else if (dTarget <= 35) { // Go catch the puck

                    speed = 0.33;
                    gearsSpeeds = getDiffParams(target->getPosX(), target->getPosY(), speed);

                    arduinoComm->gearsCommand(gearsSpeeds);
                    usleep(2000 * 1000);

                    arduinoComm->gearsCommand({0, 0});

                    cout << "Catching puck!\n";
                    arduinoComm->catchPuck();

                    if (target->getColour() == RED) {
                        leftContainer[redPucks++] = RED;
                    } else {
                        rightContainer[greenPucks++] = GREEN;
                    }

                    for (int j = 0; j < bufferSize; ++j) {
                        blobsBuffer[j] = Blob(0, 0, -1, 0);
                    }

                    usleep(3000 * 1000);        // Freeze for 3 [s] when grabbing puck

                    disableBraiten = false;

                    continue;
                }

                gearsSpeeds = getDiffParams(target->getPosX(), target->getPosY(), speed);

            }

            cout << "Number of processed frames : " << processedFrames << "\n";

        }

        braiten:
        if (millis() - braitenTime < BRAITENBERG_RATE)
            continue;
        else {

            if (!disableBraiten) {
                braitenTime = millis();

                sensors = arduinoComm->getData();
                while (!sensors.valid) {
                    cout << "Error while fetching sensors data ! Trying again in 50 [us].\n";
                    usleep(50);
                    sensors = arduinoComm->getData();
                }

                braiten(&gearsSpeeds, sensors);
                arduinoComm->gearsCommand(gearsSpeeds);
            }
        }


    }

    arduinoComm->gearsCommand({0, 0});
    cout << "Loop thread cleanly closed\n";

    pthread_exit(NULL);
}

int checkEnd() {
    return (millis() - startTime) > 119 * 1000;
}

int checkReturnTime() {
    return (millis() - startTime) > 90 * 1000;
}

void *imgProc(void *threadArgs) {

    if (CALIB) {
        //createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
        //createTrackbars(whiteBoardBounds, "WhiteBoard bounds");
    }

    int findBaseCached;

    while (!endImgProc) {

        pthread_mutex_lock(&mutexFindBase);
        findBaseCached = findBase;
        pthread_mutex_unlock(&mutexFindBase);

        capImage();

        if (findBaseCached) {

            if (findBaseCached == HOME) {
                baseImgProcess(homeBaseBounds, hsv, filtered);
                detectObjects(baseBlobs, filtered, HOME);
            } else {
                baseImgProcess(homeBaseBounds, hsv, filtered);
                detectObjects(baseBlobs, filtered, ADVERSARY);
            }

            sort(baseBlobs.begin(), baseBlobs.end(), compBlobs);

            // Lock variables to prevent (and ensure) that the blobs aren't being modified by another thread
            pthread_mutex_lock(&mutexBlobs);

            cachedBaseBlobs = baseBlobs;

            // unlock variables
            pthread_mutex_unlock(&mutexBlobs);


            // lock & unlock the counter
            pthread_mutex_lock(&mutexCount);
            acquiredFrames++;
            pthread_mutex_unlock(&mutexCount);

        } else {

            // Code to detect the board!
/*
        Mat whiteBoardFiltered;
        puckImgProcess(whiteBoardBounds, hsv, whiteBoardFiltered);
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
    }

    cout << "Image processing thread cleanly closed\n";

    pthread_exit(NULL);
}

void capBlobs() {

    puckImgProcess(hsvBoundsRed, hsv, filtered);
    detectObjects(redBlobs, filtered, RED);
    if (CALIB)
        imshow("Red filtered", filtered);


    puckImgProcess(hsvBoundsGreen, hsv, filtered);
    detectObjects(greenBlobs, filtered, GREEN);
    if (CALIB)
        imshow("Green filtered", filtered);

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
