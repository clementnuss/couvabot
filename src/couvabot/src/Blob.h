//
// Created by clement on 18.02.16.
//

#ifndef OPENCV_BLOB_H
#define OPENCV_BLOB_H


class Blob {
    int posX, posY;
    double area;

public:

    Blob(int posX, int posY, double area);

    int getPosX() const;

    int getPosY() const;

    double getArea() const;
};

#endif //OPENCV_BLOB_H
