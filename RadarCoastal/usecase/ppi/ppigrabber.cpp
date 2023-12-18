#include "ppigrabber.h"

#include <RadarEngine/constants.h>
#include <RadarEngine/global.h>

#include "qbuffer.h"
//#include "qdir.h"
#include "qimage.h"
//#include "qapplication.h"
//#include "qpixmap.h"

PPIGrabber* PPIGrabber::m_grabber{nullptr};

PPIGrabber* PPIGrabber::GetInstance(QObject *parent)
{
    if(!m_grabber) {
        m_grabber = new PPIGrabber(parent);
    }
    return m_grabber;
}

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
        QByteArray ba;
        QBuffer buf(&ba);
        QString strBase64;

        buf.open(QIODevice::WriteOnly);

        image.save(&buf, "png");

        strBase64 = QString(ba.toBase64(QByteArray::Base64Encoding));
//        qDebug()<<Q_FUNC_INFO<<"base64"<<strBase64;

        emit signalSendEcho(strBase64, image.width(), image.height());

        /* test read and save from base64 image
        QByteArray ba64 = QByteArray::fromBase64(strBase64.toUtf8());
        QPixmap img;
        img.loadFromData(ba64);
        img.save(qApp->applicationDirPath()+QDir::separator()+"base64_grab.png", "png");
        //        image.save(qApp->applicationDirPath()+"/grab.png");
        grabStart = false; //test
        */

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
