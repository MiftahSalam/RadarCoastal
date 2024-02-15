#include "wsdevicewrapper.h"

WSDeviceWrapper::WSDeviceWrapper(QObject *parent, QString config): DeviceWrapper(parent)
{
    m_config = config;
    server = nullptr;
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
    if(server->isListening())
    {
        qint64 now = QDateTime::currentSecsSinceEpoch();

        if((now-m_last_data_time) > 10) m_currentStatus = DeviceWrapper::NO_INPUT_DATA;
        else m_currentStatus = DeviceWrapper::INPUT_AVAIL;
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
    m_config = config;
    url = config;
    qDebug()<<Q_FUNC_INFO<<"config"<<m_config;

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
