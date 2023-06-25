#include "devicewrapper.h"

DeviceWrapper::DeviceWrapper(QObject *parent) :
    QObject(parent)
{
//    _currentData = "";
    last_data_time = QDateTime::currentSecsSinceEpoch();
}
