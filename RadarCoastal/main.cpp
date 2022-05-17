#include "mainwindow.h"

#include <radarconfig.h>

#include <QApplication>
#include <QDir>

QString loadStylesheetFile( const QString &path );

int main(int argc, char *argv[])
{
    RadarConfig::RadarConfig *instance = RadarConfig::RadarConfig::getInstance(QDir::homePath()+QDir::separator()+"mpsr"+QDir::separator()+".radar.conf");
    if(!instance) qFatal("Cannot provide config service");

    if(instance->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_USE_OPENGL_SOFTWARE).toBool())
        QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL); //tiap pc beda.why??
    QApplication a(argc, argv);


    QString appStyle;
    if(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_DISPLAY_PRESET_COLOR).toInt() == 0)
        appStyle = loadStylesheetFile( ":/css/HMI_style.css" );
    else if(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_DISPLAY_PRESET_COLOR).toInt() == 1)
        appStyle = loadStylesheetFile( ":/css/HMI_style_night.css" );

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
