#ifndef PLANTTHREAD_H
#define PLANTTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QPixmap>

class PlantThread : public QThread
{
    Q_OBJECT
public:
    explicit PlantThread(QObject *parent = nullptr);
    void startWork();
    void startTime();

    void setPos(int x);

protected:
    void run() override;

private:
    int posX;
    int posY;
    QString address;
    int picNum = 1;
    QTimer *timer;

signals:
    void sendPos(int x);
};

#endif // PLANTTHREAD_H
