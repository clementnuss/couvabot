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
    double area;

public:

    Blob(int posX, int posY, int area, int colour);

    int getPosX() const;

    int getPosY() const;

    double getArea() const;

    int getColour() const;

};

#endif //OPENCV_BLOB_H
