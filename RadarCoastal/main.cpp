#include "view/mainwindow.h"

#include <RadarEngine/radarconfig.h>

#include <QApplication>
#include <QDir>
#include <QTranslator>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
#include <log4qt/propertyconfigurator.h>
#include <log4qt/loggerrepository.h>

static void initializeRootLogger();
static void shutdownRootLogger();
static void logStartup();
static void logShutdown();
static void setupRootLogger(const QString &introMessage);
static void shutDownRootLogger(const QString &extroMessage);
#endif

QString loadStylesheetFile( const QString &path );
QStringList findQmFiles();
QString selectQmFile( const QString &lng );

int main(int argc, char *argv[])
{
    RadarEngine::RadarConfig *instance = RadarEngine::RadarConfig::getInstance(QDir::homePath()+QDir::separator()+".hypernet"+QDir::separator()+".radar.conf");
    if(!instance) qFatal("Cannot provide config service");

    if(instance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_USE_OPENGL_SOFTWARE).toBool())
        QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL); //tiap pc beda.why??

    QApplication a(argc, argv);

#ifdef USE_LOG4QT
    initializeRootLogger();
    logStartup();
#endif

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

    int ret;
    {
        ret = a.exec();
    }

#ifdef USE_LOG4QT
    logShutdown();
    shutdownRootLogger();
#endif

    return ret;
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

#ifdef USE_LOG4QT
void initializeRootLogger()
{
    setupRootLogger(QStringLiteral("Root logger is setup."));
}

void shutdownRootLogger()
{
    shutDownRootLogger(QStringLiteral("Root logger was shutdown."));
}

void logStartup()
{
    auto logger = Log4Qt::Logger::rootLogger();

    logger->info(QStringLiteral("################################################################"));
    logger->info(QStringLiteral("#                          START                               #"));
    logger->info(QStringLiteral("################################################################"));
}

void logShutdown()
{
    auto logger = Log4Qt::Logger::rootLogger();

    logger->info(QStringLiteral("################################################################"));
    logger->info(QStringLiteral("#                          STOP                                #"));
    logger->info(QStringLiteral("################################################################"));
}

void setupRootLogger(const QString &introMessage)
{
    QString configFile = QApplication::applicationDirPath() + QDir::separator() + QStringLiteral(".log4qt.properties");
    qDebug()<<configFile;
    if (QFile::exists(configFile))
        Log4Qt::PropertyConfigurator::configureAndWatch(configFile);
    if (!introMessage.isEmpty())
        Log4Qt::Logger::rootLogger()->info(introMessage);
}

void shutDownRootLogger(const QString &extroMessage)
{
    auto logger = Log4Qt::Logger::rootLogger();

    if (!extroMessage.isEmpty())
        logger->info(extroMessage);
    logger->removeAllAppenders();
    logger->loggerRepository()->shutdown();
}
#endif

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
