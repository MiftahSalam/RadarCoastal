#ifndef NAV_DATA_MODEL_H
#define NAV_DATA_MODEL_H

#include <QByteArray>
#include <QString>

struct NavDataModel
{
    quint8 status_gps;
    quint8 status_hdg;
    long long timestamp;
    double lat;
    double lon;
    double hdg;
    bool gps_man;
    bool hdg_man;

    NavDataModel()
    {
        timestamp = -1;
    }
};

class NavDataDecoder
{
public:
    NavDataDecoder();

    virtual ~NavDataDecoder(){}

    virtual NavDataModel decode() = 0;

    void update(QByteArray data);
    bool isGPSDataValid(const double lat, const double lon);
    bool isHDGDataValid(const double hdg);

protected:
    virtual void reset();

    QByteArray m_data;
};

class NavDataEncoder
{
public:
    NavDataEncoder(long long ts,
                      double lat,
                      double lon,
                      double hdg,
                      bool gps_man,
                      bool hdg_man
                   );
    NavDataEncoder(NavDataModel data);

    virtual ~NavDataEncoder(){}

    virtual QString encode() = 0;

protected:
    NavDataModel m_data;
};

class NavDataDecoderNMEA: public NavDataDecoder
{
public:
    NavDataDecoderNMEA();

    // NavDataDecoder interface
    NavDataModel decode() override;

protected:
    void reset() override;

private:
    QString m_append_data_osd;
};

class NavDataDecoderCustom: public NavDataDecoder
{
public:
    NavDataDecoderCustom();

    // NavDataDecoder interface
    NavDataModel decode() override;

protected:
    void reset() override;

private:
    QString m_append_data_osd;
};

class NavDataEncoderCustom: public NavDataEncoder
{
public:
    NavDataEncoderCustom(long long ts,
                          double lat,
                          double lon,
                          double hdg,
                         bool gps_man,
                         bool hdg_man
                         );
    NavDataEncoderCustom(NavDataModel data);

    // NavDataEncoder interface
    QString encode() override;
};

class NavDataEncoderJson: public NavDataEncoder
{
public:
    NavDataEncoderJson(long long ts,
                          double lat,
                          double lon,
                          double hdg,
                         bool gps_man,
                         bool hdg_man
                         );
    NavDataEncoderJson(NavDataModel data);

    // NavDataEncoder interface
    QString encode() override;
};

#endif // NAV_DATA_MODEL_H
