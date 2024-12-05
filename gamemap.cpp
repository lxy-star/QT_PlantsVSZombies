#include <QScrollBar>
#include <QTimer>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QGraphicsPixmapItem>
#include <QtGlobal>
#include <QPropertyAnimation>
#include <QList>
#include <memory> // 用于 std::unique_ptr
#include "gamemap.h"
#include "plantscard.h"
#include "gamemedia.h"
#include "cooldowndark.h"
#include "spade.h"
#include "plants.h"
#include "zombies.h"
#include "smallcar.h"

// 定义静态成员变量
int GameMap::SunshineNum = 100;
int GameMap::ZombiesNum = 0;

GameMap::GameMap(QGraphicsView *parent) : QGraphicsView(parent){
    this->setWindowTitle("刘星宇的Plants VS Zombies");
    this->gameScene = new QGraphicsScene;
    this->backgroundItem = new QGraphicsPixmapItem;
    //隐藏指针
    //this->setCursor(Qt::BlankCursor);

    //添加草地背景
    this->gameScene->setSceneRect(QRectF(0,0,1400,600));
    this->backgroundItem->setPixmap(QPixmap(":/res/backGround/gameBackground.jpg"));
    this->gameScene->addItem(this->backgroundItem);

    this->setScene(this->gameScene);
    this->setFixedSize(800,602);
    //隐藏水平拖动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->horizontalScrollBar()->setValue(288);

    //显示阳光label
    this->showSun = new QLabel;
    this->showSun->setText(QString::number(GameMap::SunshineNum));
    this->showSun->setStyleSheet("QLabel {background-color: transparent;}");
    this->showSun->setFont(QFont("Microsoft YaHei",10));
    this->showSun->resize(50,50);
    this->showSun->setAlignment(Qt::AlignHCenter);
    QTimer * updateSun = new QTimer(this);
    connect(updateSun,&QTimer::timeout,this,&GameMap::updateSun);
    updateSun->start(100);

    //显示初始站立僵尸
    this->showBeginStandZombies();

    this->showChoosePlantsWidget();
    this->gameBeginMove();
    //测试代码用  编译删除-------------------------------------------
    //this->downChooseCard->start(3);
    //------------------------------------------------------------
    //强制刷新
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void GameMap::setMouse(){
    QGraphicsPixmapItem * item = new QGraphicsPixmapItem;
    item->setPixmap(QPixmap(":/res/mouse/mouse.png"));
    this->gameScene->addItem(item);
    item->setZValue(60);
    QTimer * timer = new QTimer;
    connect(timer,&QTimer::timeout,this,[=](){
        item->setPos(Plants::mousePosX - 20,Plants::mousePosY + 29);
    });
    timer->start(6);
}

void GameMap::showChoosePlantsWidget()
{
    this->downChooseCard = new QTimer(this);
    //创建选卡窗口
    this->chooseCardWidget = new QGraphicsPixmapItem(QPixmap(":/res/backGround/chooseCardBar.png"));
    this->chooseCardWidget->setZValue(1);
    //this->chooseCardWidget->setPos(230,-3);
    this->chooseCardWidget->setPos(230,-89);
    this->gameScene->addItem(chooseCardWidget);
    showSunLabel = this->gameScene->addWidget(this->showSun);
    showSunLabel->setPos(243,60);
    showSunLabel->hide();
    showSunLabel->setZValue(2);

    //连接向下移动选择卡牌窗口
    connect(this->downChooseCard,&QTimer::timeout,this,[=](){
        this->chooseCardWidget->setPos(chooseCardWidget->x(),chooseCardWidget->y() + 1);
        if(this->chooseCardWidget->y() >= -3)
        {
            this->downChooseCard->stop();
            this->showSunLabel->show();

            //显示植物卡牌
            this->doShowPlantsCard();
        }
    });
}

void GameMap::updateSun()
{
    //检测阳光是否发生变化
    if(this->showSun->text().toInt() == GameMap::SunshineNum)
    {
        return;
    }

    this->showSun->setText(QString::number(GameMap::SunshineNum));
}

void GameMap::gameBeginMove()
{
    // 使用智能指针管理定时器和音效对象的生命周期
    this->leftMove = new QTimer(this);
    this->rightMove = new QTimer(this);
    this->horizontalScrollBar()->setValue(0);

    // 播放开头音乐
    this->beginChooseCardMedia = new GameMedia(":/res/Media/gameBeginChooseCard.wav", this);

    connect(this->rightMove, &QTimer::timeout, this, [this]() {
        this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value() + 1);
        if (this->horizontalScrollBar()->value() >= 600) {
            this->rightMove->stop();

            // 等待两秒后 镜头向左移动
            QTimer::singleShot(2300, this, [this]() {
                // 镜头向左移动
                this->leftMove->start(2);
            });
        }
    });

    connect(this->leftMove, &QTimer::timeout, this, [this]() {
        this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value() - 1);
        if (this->horizontalScrollBar()->value() <= 217) {
            this->leftMove->stop();

            // 出现选择植物框
            this->downChooseCard->start(2);

            // 显示小推车
            this->setSmallCar();
            // 显示准备开始游戏
            this->showRedReady();
            // 设置铲子
            this->setSpade();
            // 设置自然阳光
            auto setSunTimer = new QTimer(this);
            connect(setSunTimer, &QTimer::timeout, this, &GameMap::setSunshine);
            setSunTimer->start(13000);

            // 开始生成僵尸
            this->outZombies();
        }
    });

    // 开局等待两秒后 开始移动镜头
    QTimer::singleShot(2300, this, [this]() {
        this->rightMove->start(2);
    });
}

void GameMap::doShowPlantsCard()
{
    int id = 1;
    this->cards = {&Id_1,&Id_2,&Id_3,&Id_4,&Id_5,&Id_6};
    for(QVector<PlantsCard**>::iterator it = cards.begin();it!=cards.end();it++)
    {
        *(*it) = new PlantsCard(id,248 + (53 * id),6);
        this->gameScene->addItem((*(*it)));
        id ++;
        //将半透明阴影item 发送至scene游戏场景中
        connect(*(*it),&PlantsCard::sendNewItem,this,&GameMap::getNewItem);
        //连接 花费阳光信号槽
        connect(*(*it),&PlantsCard::spendSunshine,this,&GameMap::doSpendSunshine);
    }

    //将阳光传递给每张卡
    //    connect(this,&gameMap::sendSunshine,this,[=](int num){
    //        for(QVector<PlantsCard**>::iterator it = cards.begin();it!=cards.end();it++)
    //        {
    //            (*(*it))->getSunshine(num);
    //        }
    //    });
    //    QTimer * send = new QTimer;
    //    connect(send,&QTimer::timeout,this,[=](){
    //        emit this->sendSunshine(gameMap::SunshineNum);
    //    });
    //    send->start(10);
}

void GameMap::getNewItem(QGraphicsObject * item)
{
    this->scene()->addItem(item);
}

void GameMap::doSpendSunshine(int num)
{
    GameMap::SunshineNum -= num;
}

void GameMap::switchPixmap(QGraphicsPixmapItem* item, const QString& pixmapPath, const QPointF& pos)
{
    item->setPixmap(QPixmap(pixmapPath));
    item->setPos(pos);
}

void GameMap::showRedReady()
{
    // 使用智能指针管理 showReadyGame 的生命周期
    auto showReadyGame = new QGraphicsPixmapItem();
    this->gameScene->addItem(showReadyGame);

    // 音效
    std::unique_ptr<GameMedia> ready(new GameMedia(":/res/Media/ReadySetPlant.wav", this));

    switchPixmap(showReadyGame, ":/res/backGround/gameBeginRed/gameBegin-Ready.png", QPointF(500, 260));

    // 逐渐停止最开始选卡音乐
    this->beginChooseCardMedia->stopMedia(true);

    // 计时器切换图片
    QTimer::singleShot(600, this, [this, showReadyGame]() {
        switchPixmap(showReadyGame, ":/res/backGround/gameBeginRed/gameBegin-Set.png", QPointF(530, 260));
    });

    QTimer::singleShot(1200, this, [this, showReadyGame]() {
        switchPixmap(showReadyGame, ":/res/backGround/gameBeginRed/gameBegin-Plant.png", QPointF(436, 260));
    });

    QTimer::singleShot(2000, this, [this, showReadyGame]() {
        showReadyGame->setPos(0, -300);
        // 播放白天音乐
        std::unique_ptr<GameMedia> dayMusic(new GameMedia(":/res/Media/pvzDayMusic.wav", this));
        dayMusic->setForeverPlay(":/res/Media/pvzDayMusic.wav");
    });
}

void GameMap::mouseMoveEvent(QMouseEvent *event)
{
    //将鼠标坐标存储至Plants公共成员变量
    Plants::mousePosX = event->pos().x() + 190;
    Plants::mousePosY = event->pos().y() - 50;
}

void GameMap::showBeginStandZombies()
{
    Zombies * zombie = new Zombies("normalZombie",true,this->gameScene,this);
    zombie->setPos(1120,110);
    this->zombiesVector.push_back(zombie);
    //    zombie = new Zombies("icecreamZombie",false,this->gameScene,this);
    //    zombie->setPos(1130,110);
    //    this->zombiesVector.push_back(zombie);
    zombie = new Zombies("icecreamZombie",true,this->gameScene,this);
    zombie->setPos(1090,210);
    this->zombiesVector.push_back(zombie);
    zombie = new Zombies("normalZombie",true,this->gameScene,this);
    zombie->setPos(1022,312);
    this->zombiesVector.push_back(zombie);
    zombie = new Zombies("normalZombie",true,this->gameScene,this);
    zombie->setPos(1040,232);
    this->zombiesVector.push_back(zombie);
    zombie = new Zombies("normalZombie",true,this->gameScene,this);
    zombie->setPos(1060,390);
    this->zombiesVector.push_back(zombie);
    zombie = new Zombies("icecreamZombie",true,this->gameScene,this);
    zombie->setPos(1123,250);
    this->zombiesVector.push_back(zombie);
    zombie = new Zombies("normalZombie",true,this->gameScene,this);
    zombie->setPos(1198,230);
    this->zombiesVector.push_back(zombie);
}

void GameMap::deleteBeginZombie()
{
    for(QVector<Zombies *>::iterator it = this->zombiesVector.begin();it!=this->zombiesVector.end();it++)
    {
        delete *it;
    }
}

void GameMap::setSmallCar()
{
    for(int i=0;i<5;i++)
    {
        int num = i * 100;
        SmallCar * car = new SmallCar(this->gameScene,this);
        car->setPos(179,119 + num);
    }
}

void GameMap::setSpade()
{
    Spade * spade = new Spade(this->gameScene,this);
    QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem;
    pixmapItem->setPixmap(QPixmap(":/res/spade/spadeWidget.png"));
    pixmapItem->setZValue(86);
    this->gameScene->addItem(pixmapItem);
    spade->setPos(776,0);
    pixmapItem->setPos(786,0);
}

void GameMap::setSunshine()
{
    // 使用 QRandomGenerator 初始化随机数生成器
    int randomX = QRandomGenerator::global()->bounded(300, 901); // 生成 [300, 900] 范围内的随机数
    int randomY = QRandomGenerator::global()->bounded(100, 501); // 生成 [100, 500] 范围内的随机数

    this->sunshine = new Sunshine(this->gameScene);
    sunshine->natureSunshine(randomX, randomY);
}

void GameMap::outZombies()
{
    //X轴起始为 960  Y为 15 + (n * 100)

    QTimer::singleShot(20000,this, [this]() {
        int randomY = QRandomGenerator::global()->bounded(5); // 生成 [0, 4] 范围内的随机数

        std::unique_ptr<GameMedia> media(new GameMedia(":/res/Media/zombieComing.mp3", this)); // 使用 unique_ptr 管理生命周期
        auto zombie = new Zombies("normalZombie", false, this->gameScene, this);
        zombie->setPos(960, 100 * randomY + 15);
    });

    QTimer::singleShot(60000, this,[this]() {
        int randomY = QRandomGenerator::global()->bounded(5); // 生成 [0, 4] 范围内的随机数

        auto zombie1 = new Zombies("normalZombie", false, this->gameScene, this);
        zombie1->setPos(960, 100 * randomY + 15);

        randomY = QRandomGenerator::global()->bounded(5); // 再次生成新的随机数
        auto zombie2 = new Zombies("normalZombie", false, this->gameScene, this);
        zombie2->setPos(960, 100 * randomY + 15);
    });

    QTimer::singleShot(95000, this,[this]() {
        int randomY = QRandomGenerator::global()->bounded(5); // 生成 [0, 4] 范围内的随机数

        auto zombie1 = new Zombies("normalZombie", false, this->gameScene, this);
        zombie1->setPos(960, 100 * randomY + 15);

        int randomY2 = QRandomGenerator::global()->bounded(5); // 再次生成新的随机数
        if (randomY2 == randomY) {
            randomY = QRandomGenerator::global()->bounded(5); // 确保两个僵尸不在同一行
        }
        auto zombie2 = new Zombies("icecreamZombie", false, this->gameScene, this);
        zombie2->setPos(960, 100 * randomY + 15);
    });

    QTimer::singleShot(115000, this,[this]() {
        int randomY = QRandomGenerator::global()->bounded(5); // 生成 [0, 4] 范围内的随机数

        auto zombie = new Zombies("normalZombie", false, this->gameScene, this);
        zombie->setPos(960, 100 * randomY + 15);
    });

    QTimer::singleShot(115000 + 30000, this,[this]() {
        int randomY = QRandomGenerator::global()->bounded(5); // 生成 [0, 4] 范围内的随机数

        auto zombie = new Zombies("icecreamZombie", false, this->gameScene, this);
        zombie->setPos(960, 100 * randomY + 15);
    });
}
