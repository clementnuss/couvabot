//
// Created by clement on 18.02.16.
//

#include "Blob.h"
#include "camera.h"
#include <math.h>

Blob::Blob(int posX, int posY, int area, int colour) {
    this->posX = posX;
    this->posY = posY;
    this->area = area;
    this->colour = colour;
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

bool compBlobs(const Blob &b1, const Blob &b2) {
    return b1.getArea() > b2.getArea();
}

Blob::Blob() {
    posX = posY = 0;
    area = -1;
    colour = -1;
}
