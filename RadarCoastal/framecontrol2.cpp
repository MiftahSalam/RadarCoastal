#include "framecontrol2.h"
#include "ui_framecontrol2.h"

#include <radarconfig.h>

#include <QIntValidator>

FrameControl2::FrameControl2(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameControl2), m_re(nullptr)
{
    ui->setupUi(this);

    ui->lineEditGain->setValidator(new QIntValidator(0,255,ui->lineEditGain));
    ui->lineEditRain->setValidator(new QIntValidator(0,255,ui->lineEditRain));
    ui->lineEditSea->setValidator(new QIntValidator(0,255,ui->lineEditSea));
    ui->lineEditMTI->setValidator(new QIntValidator(0,255,ui->lineEditMTI));

    ui->lineEditGain->setText(QString::number(ui->horizontalSliderGain->value()));
    ui->lineEditRain->setText(QString::number(ui->horizontalSliderRain->value()));
    ui->lineEditSea->setText(QString::number(ui->horizontalSliderSea->value()));
    ui->lineEditMTI->setText(QString::number(ui->horizontalSliderMTI->value()));


}

void FrameControl2::setRadarEnginePtr(RadarEngine::RadarEngine *re)
{
    m_re = re;
}

FrameControl2::~FrameControl2()
{
    delete ui;
}

void FrameControl2::on_pushButtonGain_clicked()
{
    if(ui->pushButtonGain->isChecked())
    {
        ui->pushButtonGain->setText("Gain (Auto)");
        ui->horizontalSliderGain->setEnabled(false);
        ui->lineEditGain->setEnabled(false);

        if(m_re) m_re->trigger_ReqControlChange(RadarEngine::CT_GAIN,-1);
//        emit signal_change_gain_req(-1);
    }
    else
    {
        ui->pushButtonGain->setText("Gain (Man)");
        ui->horizontalSliderGain->setEnabled(true);
        ui->lineEditGain->setEnabled(true);
        ui->lineEditGain->setText(QString::number(ui->horizontalSliderGain->value()));
    }
}

void FrameControl2::on_horizontalSliderGain_valueChanged(int value)
{
    ui->lineEditGain->setText(QString::number(value));
//    emit signal_change_gain_req(value);
    if(m_re) m_re->trigger_ReqControlChange(RadarEngine::CT_GAIN,value);
}

void FrameControl2::on_lineEditGain_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    ui->horizontalSliderGain->setValue(value);
//    emit signal_change_gain_req(value);
    if(m_re) m_re->trigger_ReqControlChange(RadarEngine::CT_GAIN,value);
}

void FrameControl2::on_pushButtonSea_clicked()
{
    if(ui->pushButtonSea->isChecked())
    {
        ui->pushButtonSea->setText("Land (Auto)");
        ui->horizontalSliderSea->setEnabled(false);
        ui->lineEditSea->setEnabled(false);
//        emit signal_change_sea_req(-1);
        if(m_re) m_re->trigger_ReqControlChange(RadarEngine::CT_SEA,-1);
    }
    else
    {
        ui->pushButtonSea->setText("Land (Man)");
        ui->horizontalSliderSea->setEnabled(true);
        ui->lineEditSea->setEnabled(true);
        ui->lineEditSea->setText(QString::number(ui->horizontalSliderSea->value()));
    }
}
void FrameControl2::on_horizontalSliderSea_valueChanged(int value)
{
    ui->lineEditSea->setText(QString::number(value));
//    emit signal_change_sea_req(value);
    if(m_re) m_re->trigger_ReqControlChange(RadarEngine::CT_SEA,value);
}

void FrameControl2::on_lineEditSea_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    ui->horizontalSliderSea->setValue(value);
//    emit signal_change_sea_req(value);
    if(m_re) m_re->trigger_ReqControlChange(RadarEngine::CT_SEA,value);
}

void FrameControl2::on_horizontalSliderRain_valueChanged(int value)
{
    ui->lineEditRain->setText(QString::number(value));
//    emit signal_change_rain_req(value);
    if(m_re) m_re->trigger_ReqControlChange(RadarEngine::CT_RAIN,value);
}

void FrameControl2::on_lineEditRain_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    ui->horizontalSliderRain->setValue(value);
//    emit signal_change_rain_req(value);
    if(m_re) m_re->trigger_ReqControlChange(RadarEngine::CT_RAIN,value);
}

void FrameControl2::on_checkBoxMTI_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_RADAR_PARAMS_FILTER_CONTROL_MTI,checked);
//    enable_mti = checked;
    if(checked)
    {
        ui->horizontalSliderMTI->setDisabled(false);
        ui->lineEditMTI->setDisabled(false);
    }
    else
    {
        ui->horizontalSliderMTI->setEnabled(false);
        ui->lineEditMTI->setEnabled(false);
    }

}

void FrameControl2::on_lineEditMTI_textChanged(const QString &arg1)
{
    const int mti_value = arg1.toInt();
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_RADAR_PARAMS_FILTER_DATA_MTI,mti_value);
    ui->horizontalSliderMTI->setValue(mti_value);
}

void FrameControl2::on_horizontalSliderMTI_valueChanged(int value)
{
    const int mti_value = value;
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_RADAR_PARAMS_FILTER_DATA_MTI,mti_value);
//    mti_value = value;
    ui->lineEditMTI->setText(QString::number(mti_value));
}
