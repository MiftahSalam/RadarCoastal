#include "stream.h"

#include <RadarEngine/radarconfig.h>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, Stream)
#else
#include <QDebug>
#endif

Stream::Stream(QObject *parent, QString config) :
    QObject(parent)
{
    generateConfig(config);

    switch (m_config.type) {
    case MQTT:
        m_streamDevice = MqttDeviceWrapper::GetInstance(m_config.config);
    }

    if(m_streamDevice) connect(m_streamDevice,&DeviceWrapper::ReadyRead,this,&Stream::SignalReceiveData);
}

void Stream::SetConfig(const QString& config)
{
    generateConfig(config);

    if(!m_streamDevice)
    {
        switch (m_config.type) {
        case MQTT:
            m_streamDevice = MqttDeviceWrapper::GetInstance(m_config.config);
        }

        if(m_streamDevice) connect(m_streamDevice,&DeviceWrapper::ReadyRead,this,&Stream::SignalReceiveData);
    }
    else m_streamDevice->InitConfig(m_config.config);
}

void Stream::ChangeConfig(const QString &config)
{
    m_streamDevice->ChangeConfig(config);
}

void Stream::Reconnect()
{
    m_streamDevice->Reconnect();
}

void Stream::SendData(const QString &data)
{
    m_streamDevice->Write(data);
}
void Stream::generateConfig(const QString config)
{
#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<"config"<<config;
#else
    qDebug()<<Q_FUNC_INFO<<"config"<<config;
#endif

#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList config_list = config.split(";",Qt::SkipEmptyParts);
#else
    QStringList config_list = config.split(";",QString::SkipEmptyParts);
#endif
    if(config_list.size() == 3)
    {
        if(config_list.at(0).contains("mqtt",Qt::CaseInsensitive)) m_config.type = MQTT;
        else
        {
#ifdef USE_LOG4QT
            logger()->fatal()<<Q_FUNC_INFO<<"unknown stream type config"<<config;
#else
            qDebug()<<Q_FUNC_INFO<<"unknown stream type config"<<config;
            exit(1);
#endif
        }

        if(config_list.at(1).contains("InOut",Qt::CaseInsensitive)) m_config.mode = STREAM_IN_OUT;
        else if(config_list.at(1).contains("In",Qt::CaseInsensitive)) m_config.mode = STREAM_IN;
        else if(config_list.at(1).contains("Out",Qt::CaseInsensitive)) m_config.mode = STREAM_OUT;
        else
        {
#ifdef USE_LOG4QT
            logger()->fatal()<<Q_FUNC_INFO<<"unknown stream mode config"<<config;
#else
            qDebug()<<Q_FUNC_INFO<<"unknown stream mode config"<<config;
            exit(1);
#endif
        }
        m_config.config = config_list.at(2);
    }
#ifdef USE_LOG4QT
            logger()->warn()<<Q_FUNC_INFO<<"invalid config"<<config;
#else
    else qDebug()<<Q_FUNC_INFO<<"invalid config"<<config;
#endif

}

Stream::~Stream()
{
    delete m_streamDevice;
}

DeviceWrapper::DeviceStatus Stream::GetStreamStatus() const
{
    return m_streamDevice->GetStatus();
}

QString Stream::GetStreamStatusString() const
{
    return streamError;
}
