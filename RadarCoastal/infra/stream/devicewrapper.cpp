#include "devicewrapper.h"

DeviceWrapper::DeviceWrapper(QObject *parent) :
    QObject(parent)
{
//    _currentData = "";
    m_last_data_time = QDateTime::currentSecsSinceEpoch();
}
