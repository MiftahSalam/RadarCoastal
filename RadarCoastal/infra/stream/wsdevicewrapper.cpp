#include "wsdevicewrapper.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, WSDeviceWrapper)
#else
#include <QDebug>
#endif

WSDeviceWrapper::WSDeviceWrapper(QObject *parent, QString config): DeviceWrapper(parent)
{
    m_config = config;
    server = nullptr;
}

WSDeviceWrapper::~WSDeviceWrapper()
{
    for (auto client : m_clients) {
        client->abort();
    }
}

void WSDeviceWrapper::onNewConnection()
{
    QWebSocket *pSocket = server->nextPendingConnection();

    connect(pSocket, &QWebSocket::disconnected, this, &WSDeviceWrapper::onSocketDisconnected);

    m_clients << pSocket;

    qInfo()<<Q_FUNC_INFO<<"new connection on url"<<url<<": peer name ->"<<pSocket->peerName()<<", peer address ->"<<pSocket->peerAddress();
}

void WSDeviceWrapper::onSocketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qInfo()<<Q_FUNC_INFO<<"client disconnected on url"<<url<<": peer name ->"<<pClient->peerName()<<", peer address ->"<<pClient->peerAddress();

    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void WSDeviceWrapper::Write(const QString data)
{
    for (auto client : m_clients) {
        client->sendBinaryMessage(data.toUtf8());
    }
}

DeviceWrapper::DeviceStatus WSDeviceWrapper::GetStatus()
{
    if(!server) return DeviceWrapper::NOT_AVAIL;

    if(server->isListening())
    {
        m_currentStatus = DeviceWrapper::INPUT_AVAIL;
    }
    else m_currentStatus = DeviceWrapper::NOT_AVAIL;

    return  m_currentStatus;
}

void WSDeviceWrapper::Reconnect()
{
    InitConfig(m_config);

    if (server)
    {
        disconnect(server, &QWebSocketServer::newConnection, this, &WSDeviceWrapper::onNewConnection);
        server->close();
        delete server;
    }

    server = new QWebSocketServer("WebSocket Service...", QWebSocketServer::NonSecureMode, this);
    if (server->listen(QHostAddress(host), port)) {
        qInfo()<<Q_FUNC_INFO<<"web socket server listening on url"<<url;
    } else {
        qWarning()<<Q_FUNC_INFO<<"web socket server cannot listen on url"<<url<<" with error"<<server->errorString();
    }

    connect(server, &QWebSocketServer::newConnection, this, &WSDeviceWrapper::onNewConnection);

}

void WSDeviceWrapper::ChangeConfig(const QString command)
{
}

bool WSDeviceWrapper::InitConfig(const QString config)
{    
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList cfg_split = config.split("$", Qt::SkipEmptyParts);
#else
    QStringList cfg_split = config.split("$", QString::SkipEmptyParts);
#endif
    if(cfg_split.size() == 2)
    {
        m_config = config;
        url = cfg_split.at(0);

#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<"config"<<config;
#else
        qDebug()<<Q_FUNC_INFO<<"cfg_split"<<cfg_split;
#endif

#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
        QStringList url_split = url.split(":", Qt::SkipEmptyParts);
#else
        QStringList url_split = url.split(":", QString::SkipEmptyParts);
#endif
        if(url_split.size() == 2)
        {
            bool ok;

            host = url_split.at(0);
            auto portStr = url_split.at(1);
            port = portStr.toInt(&ok);

            if (!ok) {
                port = 8083;
                qWarning()<<Q_FUNC_INFO<<"invalid port"<<portStr<<". will use default 8083";
            }
        }
        else qFatal("invalid url config %s", url.toUtf8().constData());
    }
    else qFatal("invalid ws config %s", config.toUtf8().constData());

}
