#include "tlink.h"

Tlink::Tlink()
{
    this->coords = QPoint(0, 0);
    this->index = 0;
    this->capacity = 0.0;
    this->reserve = 0.0;
    this->flow = 0.0;
}

void Tlink::update(double res) {
    this->reserve -= res;
    this->flow += res;
}

QPoint Tlink::getCoords() {
    return this->coords;
}
double Tlink::getCapacity() {
    return this->capacity;
}
double Tlink::getReserve() {
    return this->reserve;
}
double Tlink::getFlow() {
    return this->flow;
}

int Tlink::getIndex()
{
    return this->index;
}

void Tlink::setCoords(QPoint coords)
{
    this->coords = coords;
}

void Tlink::setIndex(int index)
{
    this->index = index;
}

void Tlink::setCapacity(double cap)
{
    this->capacity = cap;
}

void Tlink::setReserve(double res)
{
    this->reserve = res;
}
void Tlink::setFlow(double f)
{
    this->flow = f;
}
