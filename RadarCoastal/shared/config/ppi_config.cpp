#include "ppi_config.h"

#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

const QString PPI_ARPA_SHOW = "ppi/arpa/show";

PPIConfig* PPIConfig::config = nullptr;

PPIConfig::PPIConfig()
{

}

bool PPIConfig::getShowArpa() const
{
    return showArpa;
}

void PPIConfig::setShowArpa(bool newShowArpa)
{
    showArpa = newShowArpa;
}

PPIConfig::~PPIConfig()
{

}

PPIConfig* PPIConfig::getInstance(const QString path) {
    qDebug()<<Q_FUNC_INFO<<"path"<<path;

    if(config == nullptr) {
        if (!QFile::exists(path)) {
//            throw ErrFileNotFound();
            qFatal("file not found: %s", path.toUtf8().constData());
        }

        config = new PPIConfig();
        config->setup(path);
    }

    return config;
}


void PPIConfig::setup(const QString path)
{
    QSettings configFile(path,QSettings::IniFormat);

    showArpa = configFile.value(PPI_ARPA_SHOW, true).toBool();
}


void PPIConfig::save(const QString path)
{
}
