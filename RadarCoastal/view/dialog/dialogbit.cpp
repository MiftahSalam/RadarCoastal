#include "dialogbit.h"
#include "ui_dialogbit.h"
#include "shared/config/applicationconfig.h"

#include <RadarEngine/global.h>

#include <QSettings>
#include <QTcpSocket>
#include <QHostAddress>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, DialogBIT)
#else
#include <QDebug>
#endif

DialogBIT::DialogBIT(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBIT)
{
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    navConfig = ApplicationConfig::getInstance()->getNavConfig();

    ui->setupUi(this);
    ui->pushButtonBITRadar->setText(tr("Start %1").arg("BiTE Radar"));
    ui->pushButtonBITNav->setText(tr("Start %1").arg("BiTE Nav"));
}

DialogBIT::~DialogBIT()
{
    delete ui;
}

void DialogBIT::on_pushButtonBITRadar_clicked()
{
    auto state_radar = m_instance_cfg->getConfig(RadarEngine::VOLATILE_RADAR_STATUS).toInt();

    if(state_radar == RadarEngine::RADAR_OFF)
    {
        ui->labelRadarPowerTest->setText("Fail");
        ui->labelRadarPowerTest->setStyleSheet("background-color: rgb(164,0,0);");
        ui->labelRadarConnTest->setText("Fail");
        ui->labelRadarConnTest->setStyleSheet("background-color: rgb(164,0,0);");
        ui->labelRadarOpTest->setText("Fail");
        ui->labelRadarOpTest->setStyleSheet("background-color: rgb(164,0,0);");
    }
    else if(state_radar == RadarEngine::RADAR_WAKING_UP)
    {
        ui->labelRadarPowerTest->setText("OK");
        ui->labelRadarPowerTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelRadarConnTest->setText("OK");
        ui->labelRadarConnTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelRadarOpTest->setText("OK (Waking up)");
        ui->labelRadarOpTest->setStyleSheet("background-color: rgb(78, 154, 6);");
    }
    else if(state_radar == RadarEngine::RADAR_STANDBY)
    {
        ui->labelRadarPowerTest->setText("OK");
        ui->labelRadarPowerTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelRadarConnTest->setText("OK");
        ui->labelRadarConnTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelRadarOpTest->setText("OK (Standby)");
        ui->labelRadarOpTest->setStyleSheet("background-color: rgb(78, 154, 6);");
    }
    else if(state_radar == RadarEngine::RADAR_TRANSMIT)
    {
        ui->labelRadarPowerTest->setText("OK");
        ui->labelRadarPowerTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelRadarConnTest->setText("OK");
        ui->labelRadarConnTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelRadarOpTest->setText("OK (Transmit)");
        ui->labelRadarOpTest->setStyleSheet("background-color: rgb(78, 154, 6);");
    }
}

void DialogBIT::on_pushButtonBITNav_clicked()
{
    auto status_gps = navConfig->getGPSStatus();
    auto status_hdt = navConfig->getHeadingStatus();

#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<status_gps<<status_hdt;
#else
    qDebug()<<Q_FUNC_INFO<<status_gps<<status_hdt;
#endif
    switch (status_gps) {
    case 0:
    {
        ui->labelNavPowerTest->setText("Fail");
        ui->labelNavPowerTest->setStyleSheet("background-color: rgb(164,0,0);");
        ui->labelNavConTest->setText("Fail");
        ui->labelNavConTest->setStyleSheet("background-color: rgb(164,0,0);");
        ui->labelNavOpTest->setText("Fail");
        ui->labelNavOpTest->setStyleSheet("background-color: rgb(164,0,0);");
        ui->labelNavGPSValidTest->setText("Invalid");
        ui->labelNavGPSValidTest->setStyleSheet("background-color: rgb(164,0,0);");
        ui->labelNavHdtValidTest->setText("Invalid");
        ui->labelNavHdtValidTest->setStyleSheet("background-color: rgb(164,0,0);");
    }
        break;
    case 1:
    {
        ui->labelNavPowerTest->setText("Ok");
        ui->labelNavPowerTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelNavConTest->setText("Ok");
        ui->labelNavConTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelNavOpTest->setText("No data");
        ui->labelNavOpTest->setStyleSheet("background-color: rgb(196, 160, 0);");
        ui->labelNavGPSValidTest->setText("No data");
        ui->labelNavGPSValidTest->setStyleSheet("background-color: rgb(196, 160, 0);");
        ui->labelNavHdtValidTest->setText("No data");
        ui->labelNavHdtValidTest->setStyleSheet("background-color: rgb(196, 160, 0);");
    }
        break;
    case 2:
    {
        ui->labelNavPowerTest->setText("Ok");
        ui->labelNavPowerTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelNavConTest->setText("Ok");
        ui->labelNavConTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelNavOpTest->setText("Invalid");
        ui->labelNavOpTest->setStyleSheet("background-color: rgb(164, 0, 0);");
        ui->labelNavGPSValidTest->setText("Invalid");
        ui->labelNavGPSValidTest->setStyleSheet("background-color: rgb(164,0,0);");
    }
        break;
    case 3:
    {
        ui->labelNavPowerTest->setText("Ok");
        ui->labelNavPowerTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelNavConTest->setText("Ok");
        ui->labelNavConTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelNavOpTest->setText("Ok");
        ui->labelNavOpTest->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->labelNavGPSValidTest->setText("Valid");
        ui->labelNavGPSValidTest->setStyleSheet("background-color: rgb(78, 154, 6);");
    }
        break;
    }

    switch (status_hdt) {
    case 2:
    {
        ui->labelNavHdtValidTest->setText("Invalid");
        ui->labelNavHdtValidTest->setStyleSheet("background-color: rgb(164,0,0);");
    }
        break;
    case 3:
    {
        ui->labelNavHdtValidTest->setText("Valid");
        ui->labelNavHdtValidTest->setStyleSheet("background-color: rgb(78, 154, 6);");
    }
        break;
    }

}
