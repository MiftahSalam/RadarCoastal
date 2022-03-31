#include "mainwindow.h"

#include <radarconfig.h>

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RadarConfig::RadarConfig *instance = RadarConfig::RadarConfig::getInstance(QDir::homePath()+QDir::separator()+"mpsr"+QDir::separator()+".radar.conf");
    if(!instance) qFatal("Cannot provide config service");

    MainWindow w;
    w.showFullScreen();

    return a.exec();
}
