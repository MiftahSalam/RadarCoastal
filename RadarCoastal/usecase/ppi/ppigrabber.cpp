#include "ppigrabber.h"
#include "shared/global.h"

#include <RadarEngine/shared/constants.h>

#include "qimage.h"
#include "qapplication.h"

PPIGrabber::PPIGrabber(QObject *parent)
    : QObject{parent}, currentAngle(0), grabStart(false), grabPending(false)
{
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    connect(m_instance_cfg,&RadarEngine::RadarConfig::configValueChange,this,&PPIGrabber::trigger_radarConfigChange);
}

void PPIGrabber::grab(QImage image)
{
    if(grabStart)
    {
        image.save(qApp->applicationDirPath()+"/grab.png");
        grabPending = false;
    }
    else qWarning()<<Q_FUNC_INFO<<"Grab not start";
}

bool PPIGrabber::pendingGrabAvailable() const
{
    return grabPending;
}

void PPIGrabber::update()
{
    if(grabStart)
    {
        currentAngle++;
        if(currentAngle >= LINES_PER_ROTATION)
        {
            currentAngle = 0;
            grabPending = true;
        }

//        qDebug()<<Q_FUNC_INFO<<"currentAngle"<<currentAngle<<"LINES_PER_ROTATION"<<LINES_PER_ROTATION-1;
    }
    else qWarning()<<Q_FUNC_INFO<<"Grab not start";
}

void PPIGrabber::stop()
{
    grabStart = false;
    currentAngle = 0;
    grabPending = false;
}

bool PPIGrabber::isStart() const
{
    return grabStart;
}

void PPIGrabber::start()
{
    grabStart = true;
    grabPending = false;
    currentAngle = 0;
}

void PPIGrabber::trigger_radarConfigChange(QString key, QVariant val)
{
    if(key == RadarEngine::VOLATILE_RADAR_STATUS) stateChange(val.toInt());
}

void PPIGrabber::stateChange(int state) {
    grabStart = static_cast<RadarEngine::RadarState>(state) == RadarEngine::RADAR_TRANSMIT;
}
