#include "nlink.h"

Nlink::Nlink()
{
    this->from = 0;
    this->to = 0;
    this->capacity = 0.0;
    this->reserve = 0.0;
    this->flow = 0.0;
}

Nlink::Nlink(double c, double r, int from, int to) {
    this->from = from;
    this->to = to;
    this->capacity = c;
    this->reserve = r;
    this->flow = 0.0;
}

int Nlink::getFrom()
{
    return this->from;
}

void Nlink::updateReserve(double res) {
    this->reserve -= res;
}

int Nlink::getTo()
{
    return this->to;
}

double Nlink::getReserve()
{
    return this->reserve;
}

void Nlink::setFrom(int index)
{
    this->flow = index;
}

void Nlink::setTo(int index)
{
    this->to = index;
}

void Nlink::setCapacity(double cap)
{
    this->capacity = cap;
}

void Nlink::setReserve(double res)
{
    this->reserve = res;
}

double Nlink::getCapacity() {
    return this->capacity;
}

void Nlink::setFlow(double f) {
    this->flow = f;
}

double Nlink::getFlow() {
    return this->flow;
}