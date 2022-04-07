#include "devicewrapper.h"

DeviceWrapper::DeviceWrapper(QObject *parent, QString config) :
    QObject(parent), _config(config)
{
//    _currentData = "";
    last_data_time = QDateTime::currentSecsSinceEpoch();
}
