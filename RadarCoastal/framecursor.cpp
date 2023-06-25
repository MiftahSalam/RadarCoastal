#include "framecursor.h"
#include "ui_framecursor.h"
#include "utils.h"

#include <radarconfig.h>

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

    RadarConfig::RadarConfig* instance = RadarConfig::RadarConfig::getInstance("");
    double const curRange = instance->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    double const curLat = instance->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    double const curLon = instance->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    QPointF distance = distancePolar(pos.x(),pos.y(),vp_width,vp_height,curRange,curLat,curLon);
    QPoint screen_middle(vp_width/2,vp_height/2);
    QPointF gps = pixToGPS(pos.x()-screen_middle.x(),-pos.y()+screen_middle.y(),vp_width,vp_height,curRange,curLat,curLon);
    QStringList gps_str = GPSString(gps.x(),gps.y());

//    qDebug()<<Q_FUNC_INFO<<"pos"<<pos<<"width"<<vp_width<<"height"<<vp_height;
//    qDebug()<<Q_FUNC_INFO<<"curRange"<<curRange<<"curLat"<<curLat<<"curLon"<<curLon<<"cursor"<<distance;
    ui->labelCursorRange->setText(QString::number(distance.x(),'f',2)+" Km");
    ui->labelCursorBrn->setText(QString::number(distance.y(),'f',2));
    ui->labelCursorLat->setText(gps_str.at(0));
    ui->labelCursorLon->setText(gps_str.at(1));
}

FrameCursor::~FrameCursor()
{
    delete ui;
}
