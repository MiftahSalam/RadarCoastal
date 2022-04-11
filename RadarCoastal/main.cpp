#include "mainwindow.h"

#include <radarconfig.h>

#include <QApplication>
#include <QDir>

QString loadStylesheetFile( const QString &path );

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication a(argc, argv);

    RadarConfig::RadarConfig *instance = RadarConfig::RadarConfig::getInstance(QDir::homePath()+QDir::separator()+"mpsr"+QDir::separator()+".radar.conf");
    if(!instance) qFatal("Cannot provide config service");

    QString appStyle = loadStylesheetFile( ":/css/HMI_style.css" );
    a.setStyleSheet( appStyle );

    MainWindow w;
    w.showFullScreen();
//    w.showMaximized();

    return a.exec();
}

QString loadStylesheetFile( const QString &path )
{
    /**
     * Load application stylesheet file (.qss) from given path
     * then return the contents to function caller
     */

    QFile styleFile( path );
    if( styleFile.open( QFile::ReadOnly ) )
    {
        qDebug() << "loading stylesheet file...";
        QString style = QLatin1String( styleFile.readAll() );
        return style;
    }
    else
    {
        qWarning() << path << ": css file not found!";
        return "";
    }
}
