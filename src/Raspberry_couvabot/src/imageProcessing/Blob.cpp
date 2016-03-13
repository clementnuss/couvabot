//
// Created by clement on 18.02.16.
//

#include "Blob.h"
#include <math.h>

Blob::Blob(int posX, int posY, int area, int colour) {
    this->posX = posX;
    this->posY = posY;
    this->area = area;
    this->colour = colour;

    //TODO: implement projection !!
    this->d = sqrt(pow(posX, 2) + pow(posY, 2));
}

int Blob::getPosX() const {
    return posX;
}

int Blob::getPosY() const {
    return posY;
}

double Blob::getArea() const {
    return area;
}

int Blob::getColour() const {
    return colour;
}

double Blob::getDistance() const {
    return d;
}

bool compBlobs(const Blob &b1, const Blob &b2) {
    return b1.getDistance() > b2.getDistance();
}
