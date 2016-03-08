//
// Created by clement on 18.02.16.
//

#include "Blob.h"

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
