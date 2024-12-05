
#include <QApplication>
#include "gamemap.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameMap map;
    map.show();
    return a.exec();
}
