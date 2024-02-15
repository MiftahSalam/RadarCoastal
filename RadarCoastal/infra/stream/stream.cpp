#include "stream.h"

#include <RadarEngine/radarconfig.h>

Stream::Stream(QObject *parent, QString config) :
    QObject(parent)
{
    generateConfig(config);

    switch (m_config.type) {
    case MQTT:
        m_streamDevice = MqttDeviceWrapper::GetInstance(m_config.config);
        break;
    case WS:
        m_streamDevice = new WSDeviceWrapper(this, m_config.config);
        break;
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
            break;
        case WS:
            m_streamDevice = new WSDeviceWrapper(this, m_config.config);
            break;
        }

        if(m_streamDevice) connect(m_streamDevice,&DeviceWrapper::ReadyRead,this,&Stream::SignalReceiveData);
    }
    else m_streamDevice->InitConfig(m_config.config);
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
    qDebug()<<Q_FUNC_INFO<<"config"<<config;

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    QStringList config_list = config.split(";",QString::SkipEmptyParts);
#else
    QStringList config_list = config.split(";",QString::SkipEmptyParts);
#endif
    if(config_list.size() == 3)
    {
        if(config_list.at(0).contains("mqtt",Qt::CaseInsensitive)) m_config.type = MQTT;
        else if(config_list.at(0).contains("ws",Qt::CaseInsensitive)) m_config.type = WS;
        else
        {
            qDebug()<<Q_FUNC_INFO<<"unknown stream type config"<<config;
            exit(1);
        }

        if(config_list.at(1).contains("InOut",Qt::CaseInsensitive)) m_config.mode = STREAM_IN_OUT;
        else if(config_list.at(1).contains("In",Qt::CaseInsensitive)) m_config.mode = STREAM_IN;
        else if(config_list.at(1).contains("Out",Qt::CaseInsensitive)) m_config.mode = STREAM_OUT;
        else
        {
            qDebug()<<Q_FUNC_INFO<<"unknown stream mode config"<<config;
            exit(1);
        }
        m_config.config = config_list.at(2);
    }
    else qDebug()<<Q_FUNC_INFO<<"invalid config"<<config;

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
