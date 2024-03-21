#ifndef TXTIMERCOUNTER_H
#define TXTIMERCOUNTER_H

#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QSettings>

#include <RadarEngine/radarconfig.h>

class TxTimerCounter : public QObject
{
    Q_OBJECT
public:
    TxTimerCounter(TxTimerCounter& other) = delete;
    TxTimerCounter() = delete;
    void operator=(const TxTimerCounter&) = delete;

    void Start();

    static quint64 GetCurrentElapsed();
    static TxTimerCounter* GetInstance(QObject *parent);

signals:
    void signal_updateElapsed(qint64 elapsed);

protected:
    explicit TxTimerCounter(QObject *parent = nullptr);

private slots:
    void triggerTimeOut();

private:
    QTimer *timer;
    QSettings *m_settings;
    QDateTime m_latest_time;
    RadarEngine::RadarConfig* m_instance_cfg;

    static qint64 m_current_counter;
    static TxTimerCounter *m_tx_timer;
};

#endif // TXTIMERCOUNTER_H
