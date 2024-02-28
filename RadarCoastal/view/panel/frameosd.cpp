#include "frameosd.h"
#include "ui_frameosd.h"

#include <QTime>
#include <QDoubleValidator>

FrameOSD::FrameOSD(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameOSD)
{
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    ui->setupUi(this);
    ui->label_20->hide();
    ui->label_19->hide();
    ui->lineEditHum->hide();
    ui->lineEditTemp->hide();

#ifdef DISPLAY_ONLY_MODE
    setEnabled(false);
#endif

    sensor = new NavSensor(this);
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&FrameOSD::on_timeout);

    initConfig();

    timer->start(1000);
}

void FrameOSD::initConfig()
{
    no_hdg_count = 0;
    hdg_col_normal = static_cast<quint8>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING).toInt());
    gps_col_normal = static_cast<quint8>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS).toInt());
    no_gps_count = 0;

    const double lat = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    const double lon = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    const double hdt = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();
    const bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    const bool hdg_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO).toBool();

    ui->lineEditLat->setValidator(new QDoubleValidator(-90,90,6,ui->lineEditLat));
    ui->lineEditLon->setValidator(new QDoubleValidator(-180,180,6,ui->lineEditLon));
    ui->lineEditHDG->setValidator(new QDoubleValidator(0,360,1,ui->lineEditHDG));

    ui->lineEditLat->setText(QString::number(lat,'f',6));
    ui->lineEditLon->setText(QString::number(lon,'f',7));
    ui->lineEditHDG->setText(QString::number(hdt,'f',1));
    ui->lineEditGMT->setText(QDateTime::currentDateTimeUtc().time().toString("hh:mm:ss"));
    ui->lineEditLocal->setText(QDateTime::currentDateTime().time().toString("hh:mm:ss"));

    ui->checkBoxGPS->setChecked(gps_auto);
    ui->checkBoxHDG->setChecked(hdg_auto);

    if(hdg_auto)
    {
        ui->lineEditHDG->setEnabled(false);
        ui->lineEditHDG->setStyleSheet("color: rgb(164,0,0);");
    }
    else
    {
        ui->lineEditHDG->setEnabled(true);
        ui->lineEditHDG->setStyleSheet("color: rgb(255,255,255);");
    }

    if(gps_auto)
    {
        ui->lineEditLat->setEnabled(false);
        ui->lineEditLon->setEnabled(false);
        ui->lineEditLat->setStyleSheet("color: rgb(164,0,0);");
        ui->lineEditLon->setStyleSheet("color: rgb(164,0,0);");
    }
    else
    {
        ui->lineEditLat->setEnabled(true);
        ui->lineEditLon->setEnabled(true);
        ui->lineEditLat->setStyleSheet("color: rgb(255,255,255);");
        ui->lineEditLon->setStyleSheet("color: rgb(255,255,255);");
    }
}

FrameOSD::~FrameOSD()
{
    delete ui;
}

void FrameOSD::updateGPSData()
{
#ifndef DISPLAY_ONLY_MODE
    const bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();

    if(gps_auto)
    {
#endif
        quint8 gps_col_normal_buf = static_cast<quint8>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_NAV_STATUS_GPS).toInt());
        const double lat = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
        const double lon = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();

        ui->lineEditLat->setText(QString::number(lat,'f',6));
        ui->lineEditLon->setText(QString::number(lon,'f',7));

        if(gps_col_normal_buf^gps_col_normal)
        {
            gps_col_normal = gps_col_normal_buf;
            updateGPSColor(gps_col_normal);
        }
#ifndef DISPLAY_ONLY_MODE
    }
#endif

}

void FrameOSD::updateHDGData()
{
#ifndef DISPLAY_ONLY_MODE
    const bool hdg_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO).toBool();

    if(hdg_auto)
    {
#endif
        quint8 hdg_col_normal_buf = static_cast<quint8>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_NAV_STATUS_HEADING).toInt());
        const double hdg = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

        ui->lineEditHDG->setText(QString::number(hdg,'f',1));

        if(hdg_col_normal_buf^hdg_col_normal)
        {
            hdg_col_normal = hdg_col_normal_buf;
            updateHDGColor(hdg_col_normal);
        }
#ifndef DISPLAY_ONLY_MODE
    }
#endif
}

void FrameOSD::updateModeControl()
{
    const bool gps_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool();
    const bool hdg_auto = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO).toBool();

    ui->checkBoxGPS->setChecked(gps_auto);
    ui->checkBoxHDG->setChecked(hdg_auto);
}

void FrameOSD::updateHDGColor(const int status)
{
    switch (status) {
    case 0: //offline
        ui->lineEditHDG->setStyleSheet("color: rgb(164,0,0);");
        break;
    case 1: //no data
        ui->lineEditHDG->setStyleSheet("color: rgb(196, 160, 0);");
        break;
    case 2: //data not valid
        ui->lineEditHDG->setStyleSheet("color: rgb(164,0,0);");
        break;
    case 3: //data valid
        ui->lineEditHDG->setStyleSheet("color: rgb(78, 154, 6);");
        break;
    default:
        break;
    }

}

void FrameOSD::updateGPSColor(const int status)
{
    switch (status) {
    case 0: //offline
        ui->lineEditLat->setStyleSheet("color: rgb(164,0,0);");
        ui->lineEditLon->setStyleSheet("color: rgb(164,0,0);");
        break;
    case 1: //no data
        ui->lineEditLat->setStyleSheet("color: rgb(196, 160, 0);");
        ui->lineEditLon->setStyleSheet("color: rgb(196, 160, 0);");
        break;
    case 2: //data not valid
        ui->lineEditLat->setStyleSheet("color: rgb(164,0,0);");
        ui->lineEditLon->setStyleSheet("color: rgb(164,0,0);");
        break;
    case 3: //data valid
        ui->lineEditLat->setStyleSheet("color: rgb(78, 154, 6);");
        ui->lineEditLon->setStyleSheet("color: rgb(78, 154, 6);");
        break;
    default:
        break;
    }

}

void FrameOSD::on_timeout()
{
    ui->lineEditGMT->setText(QDateTime::currentDateTimeUtc().time().toString("hh:mm:ss"));
    ui->lineEditLocal->setText(QDateTime::currentDateTime().time().toString("hh:mm:ss"));

    updateGPSData();
    updateHDGData();
    sensor->Reconnect();
    sensor->UpdateStatus();
#ifndef DISPLAY_ONLY_MODE
    if(!ui->checkBoxGPS->isChecked())
        sensor->SendData(ui->lineEditLat->text(),ui->lineEditLon->text(),ui->lineEditHDG->text());
#else
    updateModeControl();
#endif
}

void FrameOSD::on_pushButtonApply_clicked()
{
    double currentHeading_buf = ui->lineEditHDG->text().toDouble();
    double currentOwnShipLat_buf = ui->lineEditLat->text().toDouble();
    double currentOwnShipLon_buf = ui->lineEditLon->text().toDouble();
    const double currentOwnShipLat = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    const double currentOwnShipLon = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    const double currentHeading = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

    if(currentHeading_buf >=0 && currentHeading_buf <= 360)
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING,currentHeading_buf);
    else
        ui->lineEditHDG->setText(QString::number(currentHeading,'f',1));

    if(currentOwnShipLat_buf >=-90 && currentOwnShipLat_buf <= 90)
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE,currentOwnShipLat_buf);
    else
        ui->lineEditLat->setText(QString::number(currentOwnShipLat,'f',6));

    if(currentOwnShipLon_buf >=-180 && currentOwnShipLon_buf <= 180)
        m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE,currentOwnShipLon_buf);
    else
        ui->lineEditLon->setText(QString::number(currentOwnShipLon,'f',6));

    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO,ui->checkBoxHDG->isChecked());
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO,ui->checkBoxGPS->isChecked());
//    cur_hdg_auto = hdg_auto;
//    gps_auto = ui->checkBoxGPS->isChecked();

    no_hdg_count = 0;
    no_gps_count = 0;

    if(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_HEADING_AUTO).toBool())
    {
        ui->lineEditHDG->setEnabled(false);
        updateHDGColor(hdg_col_normal);
    }
    else
    {
        ui->lineEditHDG->setEnabled(true);
        ui->lineEditHDG->setStyleSheet("color: rgb(255,255,255);");
    }

    if(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_CONTROL_GPS_AUTO).toBool())
    {
        ui->lineEditLat->setEnabled(false);
        ui->lineEditLon->setEnabled(false);
        updateGPSColor(gps_col_normal);
    }
    else
    {
        ui->lineEditLat->setEnabled(true);
        ui->lineEditLon->setEnabled(true);
        ui->lineEditLat->setStyleSheet("color: rgb(255,255,255);");
        ui->lineEditLon->setStyleSheet("color: rgb(255,255,255);");
    }
}
