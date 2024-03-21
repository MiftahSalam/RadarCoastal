#include "view/mainwindow.h"

#include <RadarEngine/radarconfig.h>

#include <QApplication>
#include <QDir>
#include <QTranslator>

QString loadStylesheetFile( const QString &path );
QStringList findQmFiles();
QString selectQmFile( const QString &lng );

int main(int argc, char *argv[])
{
    RadarEngine::RadarConfig *instance = RadarEngine::RadarConfig::getInstance(QDir::homePath()+QDir::separator()+"pjs"+QDir::separator()+".radar.conf");
    if(!instance) qFatal("Cannot provide config service");

    if(instance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_USE_OPENGL_SOFTWARE).toBool())
        QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL); //tiap pc beda.why??

    QApplication a(argc, argv);

    QString appStyle;
    if(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::VOLATILE_DISPLAY_PRESET_COLOR).toInt() == 0)
        appStyle = loadStylesheetFile( ":/css/view/assets/css/HMI_style.css" );
    else if(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::VOLATILE_DISPLAY_PRESET_COLOR).toInt() == 1)
        appStyle = loadStylesheetFile( ":/css/view/assets/css/HMI_style_night.css" );

    a.setStyleSheet( appStyle );

    QTranslator translator;
    QString lng;

    lng = instance->getConfig(RadarEngine::NON_VOLATILE_APP_DISPLAY_LANGUAGE).toString();
    translator.load(selectQmFile(lng));
    qApp->installTranslator(&translator);

    MainWindow w;
    w.showFullScreen();
//    w.showMaximized();

    return a.exec();
}

QString selectQmFile( const QString &lng )
{
    QString lngFile;
    QStringList lngLst = findQmFiles();
    foreach (auto l, lngLst) {
        if (l.contains(lng)) {
            lngFile = l;
            break;
        }
    }

    return lngFile;
}

QStringList findQmFiles()
{
    QDir dir(":/translantions");
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
                                          QDir::Name);
    for (QString &fileName : fileNames)
        fileName = dir.filePath(fileName);
    return fileNames;
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
