#include "zombies.h"

#include <QPainter>
#include <QMap>
#include <QDebug>
#include "plants.h"
#include "gamemap.h"

Zombies::Zombies(QString name,bool isStand,QGraphicsScene * scene,QObject * parents)
{
    GameMap::ZombiesNum ++;
    this->isDead = false;
    this->isBeIce = false;
    this->isDelete = false;
    this->setParent(parents);
    this->m_Name = name;
    this->isStand = isStand;
    this->isEating = false;
    this->eatMedia = new GameMedia(":/res/Media/eatPlants.wav",this,true);

    //设置碰撞tip标签
    this->setToolTip("zombie");

    this->setCartoonPixmap();
    this->setZValue(93);
    scene->addItem(this);

    //强制刷新
    QTimer * update = new QTimer;
    connect(update,&QTimer::timeout,this,[=](){
        this->update();

    });
    update->start(60);

    if(!this->isStand)
    {
        this->goWalk();
        QMap<QString,int> speed = {{"normalZombie",79},{"icecreamZombie",73}};
        this->cartoonSpeed = speed.value(this->m_Name);
    }
    else
    {
        this->cartoonSpeed = 86;
    }
    QMap<QString,int> eat = {{"normalZombie",12},{"icecreamZombie",11}};
    QMap<QString,int> healthMap = {{"normalZombie",10},{"icecreamZombie",30}};
    this->allEatCartoonNum = eat.value(this->m_Name);
    this->health = healthMap.value(this->m_Name);

    this->loadZombieCartoon();
}

Zombies::~Zombies()
{
    if(this->isStand)
    {
        GameMap::ZombiesNum --;
    }
}

void Zombies::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(-23,0,this->zombiePixmap);


    //绘制碰撞
    //        painter->setPen(Qt::red); // 设置边框颜色
    //        painter->setBrush(Qt::NoBrush); // 不填充颜色
    //        painter->drawRect(boundingRect()); // 使用boundingRect()获取图形项的边界矩形并绘制
}

QRectF Zombies::boundingRect() const
{
    return QRectF(60,90,60,30);
}

void Zombies::setCartoonPixmap()
{
    QMap<QString,int> zombiePix;
    if(this->isStand)
    {
        zombiePix = {{"normalZombie",11},{"icecreamZombie",8}};
    }
    else
    {
        zombiePix = {{"normalZombie",22},{"icecreamZombie",21}};
    }
    this->allCartoonNum = zombiePix.value(this->m_Name);
}

void Zombies::loadZombieCartoon()
{
    this->cartoonTimer = new QTimer(this);
    this->cartoonNum = 1;
    this->deadCartoonNum = 1;

    connect(cartoonTimer, &QTimer::timeout, this, [this]() {
        QString basePixmapPath = ":/res/zombies/";

        if (this->isDead) {
            if (this->deadCartoonNum == 11) {
                GameMap::ZombiesNum--;
                this->setPos(3000, 1);
                // 使用 this 作为上下文对象
                QTimer::singleShot(10000, this, [this]() {
                    if (this->isDelete) {
                        return;
                    }
                    this->isDead = true;
                    delete this;
                });
                return;
            }
            this->zombiePixmap = basePixmapPath + "dead/a (" + QString::number(this->deadCartoonNum) + ").png";
            this->deadCartoonNum++;
            return;
        }

        if (this->isEating) {
            this->zombiePixmap = basePixmapPath + "eat/" + this->m_Name + "/a (" + QString::number(this->cartoonNum) + ").png";
            this->cartoonNum++;
            if (this->cartoonNum >= this->allEatCartoonNum + 1) {
                this->cartoonNum = 1;
            }
            return;
        }

        if (this->isStand) {
            this->zombiePixmap = basePixmapPath + "stand/";
        } else {
            this->zombiePixmap = basePixmapPath + "walk/";
        }

        this->zombiePixmap += this->m_Name + "/a (" + QString::number(this->cartoonNum) + ").png";
        this->cartoonNum++;
        if (this->cartoonNum == this->allCartoonNum + 1) {
            this->cartoonNum = 1;
        }
    });

    cartoonTimer->start(this->cartoonSpeed);
}

void Zombies::goWalk()
{
    this->walkTimer = new QTimer(this);
    connect(walkTimer,&QTimer::timeout,this,[=](){
        this->setPos(this->x() - 3,this->y());
        if(this->isBeIce)
        {
            walkTimer->setInterval(300);
        }
    });
    walkTimer->start(126);
}

void Zombies::doEating()
{
    //停止走路
    this->walkTimer->stop();
    this->isEating = true;
    //播放音频
    this->eatMedia->setForeverPlay(":/res/Media/eatPlants.wav");
    this->eatMedia->playMedia();
}

void Zombies::eatOver()
{
    this->walkTimer->start(126);
    this->isEating = false;
    this->eatMedia->stopForeverPlay();
    this->eatMedia->stopMedia(false);
}

void Zombies::lessHealth()
{
    this->health --;
    if(this->health == 0)
    {
        this->dead();
    }
}

void Zombies::dead()
{
    this->isEating = false;
    this->walkTimer->stop();
    this->isDead = true;
    this->eatMedia->stopForeverPlay();
    this->eatMedia->stopMedia(false);
}

void Zombies::beIce()
{
    if(!this->isBeIce)
    {
        std::unique_ptr<GameMedia> media(new GameMedia(":/res/Media/setIce.wav"));
        this->walkTimer->setInterval(360);
        this->cartoonTimer->setInterval(200);
        this->isBeIce = true;
    }
}
