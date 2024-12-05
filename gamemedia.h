#ifndef GAMEMEDIA_H
#define GAMEMEDIA_H

#include <QObject>
#include <QtMultimedia/QtMultimedia>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QTimer>


class GameMedia : public QObject
{
    Q_OBJECT
public:
    explicit GameMedia(QString address,QObject *parent = nullptr,bool dontPlay = false);
    void playMedia();
    void stopMedia(bool isSlow);
    void setForeverPlay(QString address);
    void stopForeverPlay();
    void setSlow();
private:
    QMediaPlayer * media;
    QTimer * foreverTimer;
    QAudioOutput *audioOutput;
signals:
};

#endif // GAMEMEDIA_H
