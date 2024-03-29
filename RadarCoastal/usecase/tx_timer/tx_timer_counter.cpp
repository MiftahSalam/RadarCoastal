#include "tx_timer_counter.h"
#include "shared/utils.h"

#include <RadarEngine/global.h>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, TxTimerCounter)
#else
#include <QDebug>
#endif

const QString APP = "hypernet";

TxTimerCounter* TxTimerCounter::m_tx_timer{nullptr};
QSettings set(QSettings::IniFormat, QSettings::UserScope, "RTI", APP);
qint64 TxTimerCounter::m_current_counter = set.value("counter/tx", 0).toLongLong();

TxTimerCounter* TxTimerCounter::GetInstance(QObject *parent)
{
    if(!m_tx_timer) {
        m_tx_timer = new TxTimerCounter(parent);
    }
    return m_tx_timer;
}

TxTimerCounter::TxTimerCounter(QObject *parent)
    : QObject{parent}
{
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "RTI", APP);
    m_current_counter = m_settings->value("counter/tx", 0).toLongLong();
    m_latest_time = QDateTime::currentDateTime();

#ifdef USE_LOG4QT
    logger()->info()<<Q_FUNC_INFO<<"latest counter:"<<m_current_counter;
#else
    qDebug()<<Q_FUNC_INFO<<"latest counter:"<<m_current_counter;
#endif

    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&TxTimerCounter::triggerTimeOut);
}

void TxTimerCounter::Start()
{
    timer->start(5000);
}

quint64 TxTimerCounter::GetCurrentElapsed()
{
    return m_current_counter;
}

void TxTimerCounter::triggerTimeOut()
{
    RadarEngine::RadarState cur_state = static_cast<RadarEngine::RadarState>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_RADAR_STATUS).toInt());
    auto now = QDateTime::currentDateTime();
    if (cur_state == RadarEngine::RADAR_TRANSMIT)
    {
        qint64 deltaTime = now.toSecsSinceEpoch() - m_latest_time.toSecsSinceEpoch();

        m_current_counter += deltaTime;
        m_settings->setValue("counter/tx", m_current_counter);

#ifdef USE_LOG4QT
        logger()->debug()<<Q_FUNC_INFO<<"tx elapsed:"<<Utils::TimeElapsedDisplay(m_current_counter);
#else
        qDebug()<<Q_FUNC_INFO<<"tx elapsed:"<<Utils::TimeElapsedDisplay(m_current_counter);
#endif

        emit signal_updateElapsed(m_current_counter);
    }
    m_latest_time = now;
}
