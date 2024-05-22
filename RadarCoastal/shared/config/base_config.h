#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

#include <QVariant>
#include <QString>
#include <QList>

class ConfigListener
{
public:
    virtual void configChange(const QString key, const QVariant val) = 0;
};

class BaseConfig
{
public:
    virtual void setup(const QString path) = 0;
    virtual void save(QString path) = 0;
    void attach(ConfigListener *listener) {
        listeners.append(listener);
    }
    void detach(ConfigListener *listener) {
        listeners.removeOne(listener);
    }
    void changeConfig(const QString key, const QVariant val) {
        foreach (auto listener, listeners) {
            listener->configChange(key, val);
        }
    }

private:
    QList<ConfigListener *>listeners;
};

#endif // BASE_CONFIG_H
