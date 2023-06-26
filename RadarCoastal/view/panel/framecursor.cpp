#include "framecursor.h"
#include "ui_framecursor.h"
#include "shared/utils.h"

#include <RadarEngine/radarconfig.h>

#include <QDebug>

FrameCursor::FrameCursor(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameCursor)
{
    ui->setupUi(this);
//    ui->labelCursorLat->hide();
//    ui->labelCursorLon->hide();
}
void FrameCursor::trigger_cursorMove(const QPoint pos, const int vp_width, const int vp_height)
{

    RadarEngine::RadarConfig* instance = RadarEngine::RadarConfig::getInstance("");
    double const curLat = instance->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    double const curLon = instance->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    double curRange = instance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

    switch (unit) {
    case 1:
        curRange *= KM_TO_NM;
        break;
    default:
        break;
    }

    QPointF distance = Utils::DistancePolar(pos.x(),pos.y(),vp_width,vp_height,curRange,curLat,curLon);
    QPoint screen_middle(vp_width/2,vp_height/2);
    QPointF gps = Utils::PixToGPS(pos.x()-screen_middle.x(),-pos.y()+screen_middle.y(),vp_width,vp_height,curRange,curLat,curLon);
    QStringList gps_str = Utils::GPSString(gps.x(),gps.y());

//    qDebug()<<Q_FUNC_INFO<<"pos"<<pos<<"width"<<vp_width<<"height"<<vp_height;
//    qDebug()<<Q_FUNC_INFO<<"curRange"<<curRange<<"curLat"<<curLat<<"curLon"<<curLon<<"cursor"<<distance;
    switch (unit) {
    case 0:
        ui->labelCursorRange->setText(QString::number(distance.x(),'f',2)+" Km");
        break;
    case 1:
        ui->labelCursorRange->setText(QString::number(distance.x(),'f',2)+" NM");
        break;
    default:
        break;
    }

    ui->labelCursorBrn->setText(QString::number(distance.y(),'f',2));
    ui->labelCursorLat->setText(gps_str.at(0));
    ui->labelCursorLon->setText(gps_str.at(1));
}

FrameCursor::~FrameCursor()
{
    delete ui;
}
