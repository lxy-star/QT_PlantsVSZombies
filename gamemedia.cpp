#include "gamemedia.h"
#include <QDebug>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>

GameMedia::GameMedia(QString address, QObject *parent, bool dontPlay)
    : QObject(parent), media(new QMediaPlayer(this)), audioOutput(new QAudioOutput(this)), foreverTimer(new QTimer(this))
{
    // 将 QMediaPlayer 和 QAudioOutput 关联
    media->setAudioOutput(audioOutput);

    // 设置媒体源
    this->media->setSource(QUrl::fromLocalFile(address));

    // 设置音量为50（范围是 0.0 到 1.0）
    this->audioOutput->setVolume(0.5);

    if (!dontPlay) {
        this->media->play();
    }
}

void GameMedia::playMedia()
{
    this->media->play();
}

void GameMedia::stopMedia(bool isSlow)
{
    if (!isSlow) {
        this->media->stop();
        return;
    }

    // 创建一个局部定时器来处理渐进式停止
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        qreal currentVolume = this->audioOutput->volume();
        this->audioOutput->setVolume(currentVolume - 0.02); // 每次减少 2%
        if (currentVolume <= 0.03) { // 当音量接近 0 时停止
            timer->stop();
            this->media->stop();
            timer->deleteLater(); // 清理定时器
        }
    });
    timer->start(80); // 每 80 毫秒调用一次
}

void GameMedia::setForeverPlay(QString address)
{
    connect(foreverTimer, &QTimer::timeout, this, [this, address]() {
        if (this->media->playbackState() != QMediaPlayer::PlayingState) {
            // 设置新的媒体源
            this->media->setSource(QUrl::fromLocalFile(address));

            // 设置音量为50（范围是 0.0 到 1.0）
            this->audioOutput->setVolume(0.5);

            // 开始播放
            this->media->play();
        }
    });

    foreverTimer->start(1000); // 每秒检查一次是否需要重新播放
}

void GameMedia::stopForeverPlay()
{
    this->foreverTimer->stop();
}

void GameMedia::setSlow()
{
    this->media->setPlaybackRate(0.8);
}
