#include "frameosd.h"
#include "ui_frameosd.h"

#include <radarconfig.h>

#include <QTime>

FrameOSD::FrameOSD(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameOSD)
{
    ui->setupUi(this);

    sensor = new NavSensor(this);

    no_hdg_count = 0;
    hdg_col_normal = true;
    no_gps_count = 0;
    gps_col_normal = true;

//    m_mqtt = getMQTT();

//    connect(m_mqtt,SIGNAL(messageReceived(QString)),this,SLOT(on_receive(QString)));

    const double lat = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    const double lon = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    const double hdt = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();
    const bool gps_auto = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    const bool hdg_auto = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();

    ui->lineEditLat->setValidator(new QDoubleValidator(-90,90,6,ui->lineEditLat));
    ui->lineEditLon->setValidator(new QDoubleValidator(-180,180,6,ui->lineEditLon));
    ui->lineEditHDG->setValidator(new QDoubleValidator(0,360,1,ui->lineEditHDG));

    ui->lineEditLat->setText(QString::number(lat,'f',6));
    ui->lineEditLon->setText(QString::number(lon,'f',7));
    ui->lineEditHDG->setText(QString::number(hdt,'f',1));
    ui->lineEditGMT->setText(QDateTime::currentDateTimeUtc().time().toString("hh:mm:ss"));

    ui->checkBoxGPS->setChecked(gps_auto);
    ui->checkBoxHDG->setChecked(hdg_auto);

    if(hdg_auto)
    {
        ui->lineEditHDG->setEnabled(false);
        ui->lineEditHDG->setStyleSheet("color: rgb(0,255,0);");

//        if(m_mqtt->isConnected())
//            m_mqtt->subscribe(m_mqtt->getMID(), "radar/gyro",2);

    }
    else
    {
        ui->lineEditHDG->setEnabled(true);
        ui->lineEditHDG->setStyleSheet("color: rgb(255,255,255);");

//        if(m_mqtt->isConnected())
//            m_mqtt->unsubscribe(m_mqtt->getMID(), "radar/gyro");

    }

    if(gps_auto)
    {
        ui->lineEditLat->setEnabled(false);
        ui->lineEditLon->setEnabled(false);

//        if(m_mqtt->isConnected())
//            m_mqtt->subscribe(m_mqtt->getMID(), "radar/gps",2);

    }
    else
    {
        ui->lineEditLat->setEnabled(true);
        ui->lineEditLon->setEnabled(true);

//        if(m_mqtt->isConnected())
//            m_mqtt->unsubscribe(m_mqtt->getMID(), "radar/gps");

    }
}
/*
double FrameOSD::getHDT()
{
    hdt = currentHeading;
    return hdt;
}
double FrameOSD::getSPD()
{
    return spd;
}
double FrameOSD::getCOG()
{
    return cog;
}
double FrameOSD::getSOG()
{
    return sog;
}
double FrameOSD::getLat()
{
    lat = currentOwnShipLat;
    return lat;
}
double FrameOSD::getLon()
{
    lon = currentOwnShipLon;
    return lon;
}
*/
FrameOSD::~FrameOSD()
{
    delete ui;
}

void FrameOSD::on_timeout()
{
    ui->lineEditGMT->setText(QDateTime::currentDateTimeUtc().time().toString("hh:mm:ss"));

    const bool hdg_auto = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    const bool gps_auto = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();

    if(hdg_auto)
    {
        bool hdg_col_normal_buf;
        no_hdg_count++;
        if(no_hdg_count>200)
            no_hdg_count = 11;

        if(no_hdg_count>10)
            hdg_col_normal_buf = false;
        else
            hdg_col_normal_buf = true;

        if(hdg_col_normal_buf^hdg_col_normal)
        {
            hdg_col_normal = hdg_col_normal_buf;
            if(hdg_col_normal)
            {
                ui->lineEditHDG->setStyleSheet("color: rgb(0,255,0);");
            }
            else
            {
                ui->lineEditHDG->setStyleSheet("color: rgb(255,0,0);");
            }
        }

    }

    if(gps_auto)
    {
        bool gps_col_normal_buf;
        no_gps_count++;
        if(no_gps_count>200)
            no_gps_count = 11;

        if(no_gps_count>10)
            gps_col_normal_buf = false;
        else
            gps_col_normal_buf = true;

        if(gps_col_normal_buf^gps_col_normal)
        {
            gps_col_normal = gps_col_normal_buf;
            if(gps_col_normal)
            {
                ui->lineEditLat->setStyleSheet("color: rgb(0,255,0);");
                ui->lineEditLon->setStyleSheet("color: rgb(0,255,0);");
            }
            else
            {
                ui->lineEditLat->setStyleSheet("color: rgb(255,0,0);");
                ui->lineEditLon->setStyleSheet("color: rgb(255,0,0);");
            }
        }

    }

    /*
    if(!m_mqtt->isConnected())
    {
        if(hdg_auto)
            m_mqtt->subscribe(m_mqtt->getMID(), "radar/gyro",2);

        if(gps_auto)
            m_mqtt->subscribe(m_mqtt->getMID(), "radar/gps",2);
    }
    */
}

void FrameOSD::on_pushButtonApply_clicked()
{
    double currentHeading_buf = ui->lineEditHDG->text().toDouble();
    double currentOwnShipLat_buf = ui->lineEditLat->text().toDouble();
    double currentOwnShipLon_buf = ui->lineEditLon->text().toDouble();
    const double currentOwnShipLat = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    const double currentOwnShipLon = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    const double currentHeading = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

    if(currentHeading_buf >=0 && currentHeading_buf <= 360)
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_HEADING,currentHeading_buf);
    else
        ui->lineEditHDG->setText(QString::number(currentHeading,'f',1));

    if(currentOwnShipLat_buf >=-90 && currentOwnShipLat_buf <= 90)
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LATITUDE,currentOwnShipLat_buf);
    else
        ui->lineEditLat->setText(QString::number(currentOwnShipLat,'f',6));

    if(currentOwnShipLon_buf >=-180 && currentOwnShipLon_buf <= 180)
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE,currentOwnShipLon_buf);
    else
        ui->lineEditLon->setText(QString::number(currentOwnShipLon,'f',6));

    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO,ui->checkBoxHDG->isChecked());
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_NAV_CONTROL_GPS_AUTO,ui->checkBoxGPS->isChecked());
//    cur_hdg_auto = hdg_auto;
//    gps_auto = ui->checkBoxGPS->isChecked();

    no_hdg_count = 0;
    no_gps_count = 0;

    if(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO).toBool())
    {
        ui->lineEditHDG->setEnabled(false);
        ui->lineEditHDG->setStyleSheet("color: rgb(0,255,0);");

//        if(m_mqtt->isConnected())
//            m_mqtt->subscribe(m_mqtt->getMID(), "radar/gyro",2);


    }
    else
    {
        ui->lineEditHDG->setEnabled(true);
        ui->lineEditHDG->setStyleSheet("color: rgb(255,255,255);");

//        if(m_mqtt->isConnected())
//            m_mqtt->unsubscribe(m_mqtt->getMID(), "radar/gyro");
    }

    if(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool())
    {
        ui->lineEditLat->setEnabled(false);
        ui->lineEditLon->setEnabled(false);
        ui->lineEditLat->setStyleSheet("color: rgb(0,255,0);");
        ui->lineEditLon->setStyleSheet("color: rgb(0,255,0);");

//        if(m_mqtt->isConnected())
//            m_mqtt->subscribe(m_mqtt->getMID(), "radar/gps",2);

    }
    else
    {
        ui->lineEditLat->setEnabled(true);
        ui->lineEditLon->setEnabled(true);
        ui->lineEditLat->setStyleSheet("color: rgb(255,255,255);");
        ui->lineEditLon->setStyleSheet("color: rgb(255,255,255);");

//        if(m_mqtt->isConnected())
//            m_mqtt->unsubscribe(m_mqtt->getMID(), "radar/gps");

    }
}
