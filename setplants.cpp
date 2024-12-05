#include "setplants.h"
#include <QDebug>

SetPlants::SetPlants(QObject *parent)
    : QObject{parent}
{}

QVector<int> SetPlants::setPlantInSquare(int x,int y)
{
    QVector<int> vector;
    int Xnum = x - 250;
    int Ynum = y - 80;

    Xnum /= 80;
    Ynum /= 100;

    int num = 1;
    for(QVector<int>::iterator it = SetPlants::isSetPlant.begin();it!=SetPlants::isSetPlant.end();it++)
    {
        if(num == 1)
        {
            num = 2;
            if(*it == Xnum && *(it+1) == Ynum)
            {
                vector = {-1,-1};
                return vector;
            }
        }
        else
        {
            num = 1;
        }
    }
    SetPlants::isSetPlant.push_back(Xnum);
    SetPlants::isSetPlant.push_back(Ynum);

    Xnum *= 80;
    Ynum *= 100;
    int posX = 260 + Xnum;
    int posY = 90 + Ynum;

    vector = {posX,posY};

    //    qDebug()<<"---------";
    //    for(QVector<int>::iterator it = setPlants::isSetPlant.begin();it!=setPlants::isSetPlant.end();it++)
    //    {
    //        qDebug()<<*it;
    //    }
    //    qDebug()<<"---------";

    return vector;
}

void SetPlants::deletePos(int x, int y)
{
    // 计算网格坐标
    int gridX = (x - 250) / 80;
    int gridY = (y - 80) / 100;

    // 查找并移除对应的植物位置
    for (auto it = SetPlants::isSetPlant.begin(); it != SetPlants::isSetPlant.end(); ) {
        auto nextIt = std::next(it);
        if (nextIt != SetPlants::isSetPlant.end() && *it == gridX && *nextIt == gridY) {
            it = SetPlants::isSetPlant.erase(it, std::next(nextIt));
        } else {
            ++it;
        }
    }
}

QVector<int> SetPlants::isSetPlant;
