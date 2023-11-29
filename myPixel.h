#ifndef MYPIXEL_H
#define MYPIXEL_H
#pragma once
#include <QtWidgets>

class myPixel
{
private:
    QPoint coords;
    QColor color;

    int pixelGroup;

    int neighboursNum;

    int prev;

    QVector<int> neighbours;

public:
    myPixel();
    myPixel(QColor color, QPoint coords);
    QPoint nLinkTo;
    QColor getColor();
    QPoint getCoords();
    int getColorR();
    int getColorG();
    int getColorB();
    void setCoords(QPoint xy);
    void setColor(QColor color);
    void setPixelGroup(int group);
    int getPixelGroup();
    int getX();
    int getY();

    int getNeighbourAtIndex(int index);
    void appendNeighbour(int index);

    int getNeighboursNum();
    void setNeighboursNum(int n);

    int getPrev();
    void setPrev(int prev);

    double calcIntensity();
};

#endif // MYPIXEL_H
