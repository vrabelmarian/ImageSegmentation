#pragma once
#ifndef NLINK_H
#define NLINK_H
#include <QtWidgets>

class Nlink
{
private:

    int from; 
    int to;
    double capacity;
    double reserve;
    double flow;



public:
    Nlink();
    Nlink(double c, double r, int from, int to);

    int getFrom();
    int getTo();

    void setFrom(int index);
    void setTo(int index);


    double getCapacity();
    double getReserve();

    void setCapacity(double c);
    void setReserve(double r);

    void setFlow(double f);
    double getFlow();

    void updateReserve(double res);

};

#endif // NLINK_H
