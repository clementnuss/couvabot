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
HSVbounds hsvBoundsGreen, hsvBoundsRed, whiteBoardBounds, base1Bounds, base2Bounds;
ardCom::arduinoComm *arduinoComm;
SPICom *spiCom;
Mat img, hsv, filtered;
vector<Blob> redBlobs, greenBlobs, processedRedBlobs, processedGreenBlobs, baseBlobs, cachedBaseBlobs;

gearsPower gearsSpeeds({0, 0});

pthread_mutex_t mutexBlobs, mutexCount, mutexFindBase;
bool endImgProc, disableBraiten;
int findBase = 0, home;

unsigned int acquiredFrames, processedFrames, braitenTime;
unsigned startTime;
int leftContainer[2], rightContainer[2];

int main(int argc, char **argv) {

    hsvBoundsRed.hMin = 0;
    hsvBoundsRed.hMax = 21;
    hsvBoundsRed.sMin = 149;
    hsvBoundsRed.sMax = 224;
    hsvBoundsRed.vMin = 106;
    hsvBoundsRed.vMax = 217;

    base1Bounds.hMin = 0;
    base1Bounds.hMax = 180;
    base1Bounds.sMin = 0;
    base1Bounds.sMax = 360;
    base1Bounds.vMin = 0;
    base1Bounds.vMax = 360;

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

    startTime = millis();

    // initialize the camera
    initCam();
    usleep(500000);

    int rc = 0;
    pthread_t threads[3];
    rc = pthread_create(&threads[0], NULL, imgProc, NULL);
    if (rc) {
        cout << "Error:unable to create the image processing thread. error code: " << rc << endl;
        exit(-1);
    } else
        cout << "image processing thread launched\n";
    usleep(200000); // wait 200 [ms] to make sure the imgProcessing thread has started

    rc = pthread_create(&threads[1], NULL, control, NULL);
    if (rc) {
        cout << "Error:unable to create the control thread. error code: " << rc << endl;
        exit(-1);
    } else
        cout << "control thread launched\n";
    usleep(200000); // wait 200 [ms] to sure the control thread has started

    int tmp = 0;
    scanf("%d", &tmp);
    cout << "End of program after " << millis() - startTime << " [ms]\n";
    cout << "number of processed frames n: " << acquiredFrames << "\n";
    endImgProc = true;

    pthread_cancel(threads[0]); // End the image processing thread
    pthread_cancel(threads[1]); // End the control thread

    arduinoComm->gearsCommand({0, 0});

    if (RPI) {
        bcm2835_spi_end();
        bcm2835_close();
    }

    return 0;
}

void *control(void *threadArgs) {

    int const middleX = FRAME_WIDTH / 2;

    ardCom::sensorsData sensors;
    braitenTime = millis(); // records the last moment the braitenberg algorithm was executed
    int catchState = 0;

    unsigned int acqFrames = 0;
    int leftPucks = 0, rightPucks = 0;

    int const bufferSize = 10;
    Blob blobsBuffer[bufferSize];   // buffer, in order to compensate for non detection of the blobs on certain frames
    for (int i = 0; i < bufferSize; ++i) {
        blobsBuffer[i] = Blob(0, 0, -1, 0);
    }

    // Start sequence : wait the start signal from the arduino
    spiCom->CS0_transfer('S');
    usleep(50);

    while (arduinoComm->start() == 0) {
        // Waiting the start signal
    }

    cout << "Start signal received\n\n";
    startTime = millis();


    /**

     Base detection proces.

    pthread_mutex_lock(&mutexFindBase);
    findBase = BASE1;   // Tells the image processing thread to look for the first registered colour.
    pthread_mutex_unlock(&mutexFindBase);

    arduinoComm->gearsCommand({0.5, 0.5});
    usleep(3000 * 1000);    // Go forward for 3000 [ms]

    arduinoComm->gearsCommand({-0.6, 0.6});
    usleep(3000 * 1000);    // 180° turn for 3000 [ms]
    arduinoComm->gearsCommand({0, 0});

    while (millis() - startTime < 7000) {   // Wait for 7 seconds in order to find the base
        pthread_mutex_lock(&mutexCount);
        acqFrames = acquiredFrames;
        pthread_mutex_unlock(&mutexCount);

        if (acqFrames > processedFrames) {    // New processed image
            processedFrames = acqFrames;

            // Lock variables to prevent (and ensure) that the blobs aren't being modified by another thread
            pthread_mutex_lock(&mutexBlobs);
            Blob *base;

            if (processedGreenBlobs.size() > 0) {
                base = &cachedBaseBlobs.at(0);  // Get blob with biggest area
            } else {
                base = new Blob();
            }
            pthread_mutex_unlock(&mutexBlobs);

            if (base->getArea() >= 2500){
                home = BASE1;
                goto start;
            }
        }
    }

    if (!home) {
        home = BASE2;
    }
     */

    findBase = 0;

    start:
    while (!checkEnd()) {

        if (checkReturnTime()) {
            pthread_mutex_lock(&mutexFindBase);
            findBase = home;
            pthread_mutex_unlock(&mutexFindBase);
        }

        pthread_mutex_lock(&mutexCount);
        acqFrames = acquiredFrames;
        pthread_mutex_unlock(&mutexCount);

        if (acqFrames > processedFrames) {    // New processed image
            processedFrames = acqFrames;

            if (findBase || (leftPucks == 1 && rightPucks == 1)) {   // When there are 4 pucks, go to the base
/*
                if (leftPucks == 1 && rightPucks == 1) {
                    pthread_mutex_lock(&mutexFindBase);
                    findBase = home;
                    pthread_mutex_unlock(&mutexFindBase);

                    leftPucks = 0;   // Reset the puck counters
                    rightPucks = 0;

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
                    } else if (dTarget > 35) {
                        speed = 0.35;
                    } else if (dTarget <= 35) {
                        speed = 0.33;

                        gearsSpeeds = {speed, speed};

                        cout << "Approching base!\n";

                        unsigned int approachTime = millis();
                        while (millis() - approachTime < 1500 * 1000) {    // Approach base for 1500 [ms]
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
                        }

                        arduinoComm->gearsCommand({0, 0});

                        arduinoComm->releasePuck();

                        // clear buffer
                        for (int j = 0; j < bufferSize; ++j) {
                            blobsBuffer[j] = Blob(0, 0, -1, 0);
                        }

                        usleep(150 * 1000);

                        pthread_mutex_lock(&mutexFindBase);
                        findBase = 0;       // We will again be in the "normal" loop
                        pthread_mutex_unlock(&mutexFindBase);

                        continue;
                    }

                    // Base searching

                    gearsSpeeds = {speed, speed};
                    goto braiten;
                }
                */
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
                } else if (dTarget > 35) {
                    speed = 0.35;
                    disableBraiten = true;

                    if (target->getColour() == RED) {
                        if (leftPucks == 1) {
                            if (rightPucks == 0) {
                                arduinoComm->prepareRight();
                                catchState = RIGHT;
                                cout << "prepared right container for red puck, because red container was full";
                            } else {
                                pthread_mutex_lock(&mutexFindBase);
                                findBase = home;
                                pthread_mutex_unlock(&mutexFindBase);
                                cout << "Container full -> go home";
                                goto braiten;
                            }

                        } else {
                            arduinoComm->prepareLeft();
                            catchState = LEFT;
                        }
                        cout << "Prepared container for red puck\n";
                    } else {
                        if (rightPucks == 1) {
                            if (leftPucks == 0) {
                                arduinoComm->prepareLeft();
                                catchState = LEFT;
                                cout << "prepared left container for green puck, because green container was full";
                            } else {
                                pthread_mutex_lock(&mutexFindBase);
                                findBase = home;
                                pthread_mutex_unlock(&mutexFindBase);
                                cout << "Container full -> go home";
                                goto braiten;
                            }

                        } else {
                            arduinoComm->prepareRight();
                            catchState = RIGHT;
                        }

                        cout << "Prepared container for red puck\n";
                    }
                } else if (dTarget <= 35) { // Go catch the puck.

                    speed = 0.33;
                    gearsSpeeds = getDiffParams(target->getPosX(), target->getPosY(), speed);

                    arduinoComm->gearsCommand(gearsSpeeds);
                    usleep(2000 * 1000);

                    arduinoComm->gearsCommand({0, 0});

                    cout << "Catching puck!\n";
                    arduinoComm->catchPuck();

                    if (catchState == RIGHT) {
                        rightContainer[rightPucks++] = target->getColour();
                    } else {
                        leftContainer[leftPucks++] = target->getColour();
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
#if RPI
                sensors = arduinoComm->getData();
                while (!sensors.valid) {
                    cout << "Error while fetching sensors data ! Trying again in 50 [us].\n";
                    usleep(50);
                    sensors = arduinoComm->getData();
                }

                braiten(&gearsSpeeds, sensors);
                arduinoComm->gearsCommand(gearsSpeeds);
#endif
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
        createTrackbars(hsvBoundsGreen, "Green Trackbars");
        createTrackbars(hsvBoundsRed, "Red Trackbars");
        createTrackbars(base1Bounds, "Base 1 bounds");
        createTrackbars(base2Bounds, "Base 2 bounds");
    }

    int findBaseCached;

    while (!endImgProc) {

        pthread_mutex_lock(&mutexFindBase);
        findBaseCached = findBase;
        pthread_mutex_unlock(&mutexFindBase);

        capImage();

        if (findBaseCached) {

            if (CALIB) {
                //imshow("camera", img);
                imshow("HSV image", hsv);
            }

            if (findBaseCached == BASE1) {
                baseImgProcess(base1Bounds, hsv, filtered);
                detectObjects(baseBlobs, filtered, BASE1);
            } else {
                baseImgProcess(base2Bounds, hsv, filtered);
                detectObjects(baseBlobs, filtered, BASE2);
            }

            if (CALIB) {
                imshow("filtered", filtered);
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

            if (CALIB) {
                //imshow("camera", img);
                imshow("HSV image", hsv);
            }

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
        }

        if (CALIB)
            waitKey(100);
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
