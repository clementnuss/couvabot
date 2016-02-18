//
// Created by clement on 18.02.16.
//

#include "Blob.h"

Blob::Blob(int posX, int posY, double area) {
    this->posX = posX;
    this->posY = posY;
    this->area = area;
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
