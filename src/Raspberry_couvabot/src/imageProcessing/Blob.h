//
// Created by clement on 18.02.16.
//

#ifndef OPENCV_BLOB_H
#define OPENCV_BLOB_H

enum {
    GREEN = 1, RED = 2
};

class Blob {
    int colour;
    int posX, posY;
    double area, d;

public:

    Blob(int posX, int posY, int area, int colour);

    int getPosX() const;

    int getPosY() const;

    double getArea() const;

    int getColour() const;

    double getDistance() const;
};

bool compBlobs(const Blob &b1, const Blob &b2);

#endif //OPENCV_BLOB_H
