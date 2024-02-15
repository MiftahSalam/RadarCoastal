#ifndef WSDEVICEWRAPPER_H
#define WSDEVICEWRAPPER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>

#include "devicewrapper.h"

class WSDeviceWrapper: public DeviceWrapper
{
public:
    WSDeviceWrapper(QObject *parent, QString config);

    // DeviceWrapper interface
public:
    void Write(const QString data) override;
    DeviceStatus GetStatus() override;
    void Reconnect() override;
    void ChangeConfig(const QString command) override;
    bool InitConfig(const QString config) override;

private slots:
    void onNewConnection();
    void onSocketDisconnected();

private:
    QWebSocketServer *server;
    QList<QWebSocket*> m_clients;
    QString url;
    QString host;
    int port;

};

#endif // WSDEVICEWRAPPER_H
