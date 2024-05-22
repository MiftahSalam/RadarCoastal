#ifndef ARPACONFIG_H
#define ARPACONFIG_H

#include "base_config.h"

const QString ARPA_NET_SPASI = "Arpa/spasi/net";
const QString ARPA_NET_INTERNAL = "Arpa/internal/net";
const QString ARPA_CREATE_ARPA_BY_CLICK = "Arpa/control/create_arpa_by_click";

class ArpaConfig: public BaseConfig
{
public:    
    ArpaConfig(ArpaConfig &other) = delete;
    void operator=(const ArpaConfig&) = delete;
    ~ArpaConfig();

    static ArpaConfig* getInstance(const QString path);

    QString getMqttSpasi() const;

    QString getMqttInternal() const;
    void setMqttInternal(const QString &newMqttInternal);

    bool getCreateArpaByClick() const;

    // BaseConfig interface
    void setup(const QString path) override;
    void save(const QString path) override;


protected:
    ArpaConfig();

private:
    static ArpaConfig *config;
    QString mqttSpasi;
    QString mqttInternal;
    bool createArpaByClick;

};

#endif // ARPACONFIG_H
