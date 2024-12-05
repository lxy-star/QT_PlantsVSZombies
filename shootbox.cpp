#include "shootbox.h"

#include <QVector>
#include <QList>
#include <QGraphicsItem>
#include <QTimer>
#include <QPainter>
#include <QGraphicsScene>
#include "setplants.h"

ShootBox::ShootBox(bool isShooter,QGraphicsObject * plant,QObject * parent)
{
    this->setZValue(70);
    this->isHaveZombie = false;
    this->setParent(parent);
    this->isShooter = isShooter;
    this->plantItem = static_cast<Plants *> (plant);
    this->setPos(this->plantItem->x(),this->plantItem->y());
    connect(this,&ShootBox::shootBegin,this->plantItem,&Plants::shoot);
    connect(this,&ShootBox::shootEnd,this->plantItem,&Plants::stopShoot);

    //加入到场景中
    QGraphicsScene * scene = this->plantItem->scene();
    scene->addItem(this);

    //检测碰撞
    QTimer * check = new QTimer(this);
    connect(check,&QTimer::timeout,this,&ShootBox::checkColliding);
    check->start(100);
}

void ShootBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //    painter->setPen(Qt::blue); // 设置边框颜色
    //    painter->setBrush(Qt::NoBrush); // 不填充颜色
    //    painter->drawRect(boundingRect()); // 使用boundingRect()获取图形项的边界矩形并绘制
}

QRectF ShootBox::boundingRect() const
{
    if(this->isShooter)
    {
        return QRectF(30,10,719,13);
    }
    return QRectF(30,0,60,30);
}

void ShootBox::checkColliding()
{
    QList<QGraphicsItem *> list = this->collidingItems();
    for(QList<QGraphicsItem *>::iterator it = list.begin();it!=list.end();it++)
    {
        if((*it)->toolTip() == "zombie")
        {
            if(this->isHaveZombie)
            {
                return;
            }
            this->isHaveZombie = true;
            emit this->shootBegin();
            return;
        }
    }
    emit this->shootEnd();
    this->isHaveZombie = false;
}
