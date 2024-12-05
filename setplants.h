#ifndef SETPLANTS_H
#define SETPLANTS_H

#include <QObject>
#include <QVector>
#include <QMap>

class SetPlants : public QObject
{
    Q_OBJECT
public:
    explicit SetPlants(QObject *parent = nullptr);
    static QVector<int> setPlantInSquare(int x,int y);
    static void deletePos(int x,int y);

private:
    static QVector<int> isSetPlant;

signals:

};

#endif // SETPLANTS_H
