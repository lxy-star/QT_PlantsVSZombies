#include "cooldowndark.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QMediaPlayer>
#include <QAudioOutput>

CoolDownDark::CoolDownDark(int x, int y, int time, QObject *parent)
    : QGraphicsObject(), goUp(new QTimer(this)), upTime(time / 76)
{
    connect(goUp, &QTimer::timeout, this, &CoolDownDark::onTimeout);

    // 初始化媒体播放器和音频输出
    media = std::make_unique<QMediaPlayer>(this);
    audioOutput = std::make_unique<QAudioOutput>(this);
    media->setAudioOutput(audioOutput.get());

    // 设置音量（0-1范围）
    audioOutput->setVolume(0.5); // 50% 音量

    // 设置媒体源并播放
    media->setSource(QUrl("qrc:/res/Media/sunshineNotEnoughMusic.mp3"));
    media->play();

    setPos(x, y);
    setZValue(23);

    goUp->start(upTime);
}

void CoolDownDark::onTimeout()
{
    setY(y() - 1);
    if (y() <= -70) {
        goUp->stop();
        emit CoolDownEnd();
    }
}

void CoolDownDark::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(0, 0, QPixmap(":/res/plantsCard/darkCard/coolDownDark.png"));
}

QRectF CoolDownDark::boundingRect() const
{
    return QRectF(0, 0, 50, 69);
}
