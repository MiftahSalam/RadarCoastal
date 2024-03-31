#include "framecursor.h"
#include "ui_framecursor.h"
#include "shared/utils.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, FrameCursor)
#else
#include <QDebug>
#endif

FrameCursor::FrameCursor(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameCursor)
{
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    ui->setupUi(this);
//    ui->labelCursorLat->hide();
//    ui->labelCursorLon->hide();
}
void FrameCursor::trigger_cursorMove(const QPoint pos, const int vp_width, const int vp_height)
{

    double const curLat = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    double const curLon = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    double curRange = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    /*
    const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());
    switch (unit) {
    case 1:
        curRange *= KM_TO_NM;
        break;
    default:
        break;
    }
    */
    QPointF distance = Utils::DistancePolar(pos.x(),pos.y(),vp_width,vp_height,curRange,curLat,curLon);
    QPoint screen_middle(vp_width/2,vp_height/2);
    QPointF gps = Utils::PixToGPS(pos.x()-screen_middle.x(),-pos.y()+screen_middle.y(),vp_width,vp_height,curRange,curLat,curLon);
    QStringList gps_str = Utils::GPSString(gps.x(),gps.y());

#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"pos"<<pos.x()<<pos.y()<<"width"<<vp_width<<"height"<<vp_height;
    logger()->trace()<<Q_FUNC_INFO<<"curRange"<<curRange<<"curLat"<<curLat<<"curLon"<<curLon<<"cursor"<<distance.x()<<distance.y();
#endif
    ui->labelCursorRange->setText(Utils::RangeDisplay(distance.x()*1000., Utils::TWO_PRECISION));
    ui->labelCursorBrn->setText(QString::number(distance.y(),'f',Utils::TWO_PRECISION));
    ui->labelCursorLat->setText(gps_str.at(0));
    ui->labelCursorLon->setText(gps_str.at(1));
}

FrameCursor::~FrameCursor()
{
    delete ui;
}
