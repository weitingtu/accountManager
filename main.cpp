#include "wttmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WTTMainWindow w;
    w.show();

    return a.exec();
}
