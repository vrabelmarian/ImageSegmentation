#ifndef TLINK_H
#define TLINK_H
#pragma once
#include <QtWidgets>

class Tlink
{
private:
    QPoint coords;
    int index;
    double capacity;
    double reserve;
    double flow;

public:
    QPoint getCoords();

    void update(double res);

    double getCapacity();
    double getReserve();
    double getFlow();


    int getIndex();

    void setCoords(QPoint coords);
    void setIndex(int index);

    void setCapacity(double cap);
    void setReserve(double res);
    void setFlow(double f);

    Tlink();
};

#endif // TLINK_H
