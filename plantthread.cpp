#include "plantthread.h"

PlantThread::PlantThread(QObject *parent)
    : QThread(parent), posX(0), posY(0), timer(new QTimer(this))
{
}

void PlantThread::run()
{
    moveToThread(this); // 确保所有槽和信号都在这个线程中运行
    connect(timer, &QTimer::timeout, this, &PlantThread::startWork);
    timer->start(10);
    exec();
}

void PlantThread::startWork()
{
    this->posX += 2;
    emit sendPos(this->posX);
}

void PlantThread::setPos(int x)
{
    this->posX = x;
}
