#ifndef COOLDOWNDARK_H
#define COOLDOWNDARK_H

#include <QObject>
#include <QGraphicsObject>
#include <QtMultimedia/QtMultimedia>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QTimer>

class CoolDownDark : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CoolDownDark(int x, int y, int time, QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

signals:
    void CoolDownEnd();

private slots:
    void onTimeout();

private:
    std::unique_ptr<QMediaPlayer> media;
    std::unique_ptr<QAudioOutput> audioOutput;
    QTimer *goUp;
    int upTime;
};

#endif // COOLDOWNDARK_H
