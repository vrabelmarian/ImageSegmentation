#include "myPixel.h"

myPixel::myPixel()
{

}

myPixel::myPixel(QColor color, QPoint coords) {
    this->color = color;
    this->coords = coords;
}

QColor myPixel::getColor()
{
    return color;
}

int myPixel::getColorR()
{
    return color.red();
}

int myPixel::getColorG()
{
    return color.green();
}

int myPixel::getColorB()
{
    return color.blue();
}

QPoint myPixel::getCoords()
{
    return coords;
}

void myPixel::setCoords(QPoint coords)
{
    this->coords = coords;
}

void myPixel::setColor(QColor color)
{
    this->color = color;
}

void myPixel::setPixelGroup(int group)
{
    this->pixelGroup = group;
}

int myPixel::getPixelGroup()
{
    return this->pixelGroup;
}

int myPixel::getX() {
    return this->coords.x();
}

int myPixel::getY() {
    return this->coords.y();
}

int myPixel::getNeighbourAtIndex(int index)
{
    return this->neighbours[index];
}

void myPixel::appendNeighbour(int index)
{
    this->neighbours.append(index);
}

int myPixel::getNeighboursNum()
{
    return this->neighbours.size();
}

void myPixel::setNeighboursNum(int n)
{
    this->neighboursNum = n;
}

int myPixel::getPrev()
{
    return prev;
}
void myPixel::setPrev(int prev) {
    this->prev = prev;
}

double myPixel::calcIntensity() {
    return ((color.red() + color.green() + color.blue()) / 3);
}