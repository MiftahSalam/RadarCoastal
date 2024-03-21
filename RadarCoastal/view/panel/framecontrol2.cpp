#include "framecontrol2.h"
#include "ui_framecontrol2.h"

#include <QIntValidator>

FrameControl2::FrameControl2(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameControl2)
{
    ui->setupUi(this);
    ui->pushButtonGain->setText(tr("Gain %1").arg("(Man)"));
    ui->pushButtonSea->setText(tr("Sea %1").arg("(Man)"));
    ui->pushButtonRain->setText(tr("Rain"));

#ifdef DISPLAY_ONLY_MODE
    setEnabled(false);
#endif

    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    m_instance_re = RadarEngine::RadarEngine::GetInstance(this);

    ui->lineEditGain->setValidator(new QIntValidator(0,255,ui->lineEditGain));
    ui->lineEditRain->setValidator(new QIntValidator(0,255,ui->lineEditRain));
    ui->lineEditSea->setValidator(new QIntValidator(0,255,ui->lineEditSea));
    ui->lineEditMTI->setValidator(new QIntValidator(0,255,ui->lineEditMTI));

    ui->lineEditGain->setText(QString::number(ui->horizontalSliderGain->value()));
    ui->lineEditRain->setText(QString::number(ui->horizontalSliderRain->value()));
    ui->lineEditSea->setText(QString::number(ui->horizontalSliderSea->value()));
    ui->lineEditMTI->setText(QString::number(ui->horizontalSliderMTI->value()));

    ui->horizontalSliderVRM->setMinimum(100);
    ui->horizontalSliderVRM->setMaximum(distanceList.first());
    ui->horizontalSliderVRM->setSingleStep(distanceList.last()/100);
    ui->horizontalSliderVRM->setPageStep(distanceList.last()/1000);
}

void FrameControl2::initConfig()
{
    ui->checkBoxMTI->setChecked(m_instance_cfg->getConfig(RadarEngine::VOLATILE_RADAR_PARAMS_FILTER_CONTROL_MTI).toBool());
    ui->checkBoxEBL->setChecked(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_EBL_MARKER).toBool());
    ui->checkBoxVRM->setChecked(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_VRM_MARKER).toBool());
    ui->horizontalSlideEBL->setValue(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_EBL_VALUE).toDouble());
    ui->horizontalSliderVRM->setValue(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_VRM_VALUE).toDouble());
    ui->lineEditEBL->setText(QString::number(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_EBL_VALUE).toInt()));
    ui->lineEditVRM->setText(QString::number(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_VRM_VALUE).toInt()));
}

FrameControl2::~FrameControl2()
{
    delete ui;
}

void FrameControl2::on_pushButtonGain_clicked()
{
    if(ui->pushButtonGain->isChecked())
    {
        ui->pushButtonGain->setText(tr("Gain %1").arg("(Auto)"));
        ui->horizontalSliderGain->setEnabled(false);
        ui->lineEditGain->setEnabled(false);

        if(m_instance_re) m_instance_re->TriggerReqControlChange(RadarEngine::CT_GAIN,-1);
//        emit signal_change_gain_req(-1);
    }
    else
    {
        ui->pushButtonGain->setText(tr("Gain %1").arg("(Man)"));
        ui->horizontalSliderGain->setEnabled(true);
        ui->lineEditGain->setEnabled(true);
        ui->lineEditGain->setText(QString::number(ui->horizontalSliderGain->value()));
    }
}

void FrameControl2::on_horizontalSliderGain_valueChanged(int value)
{
    ui->lineEditGain->setText(QString::number(value));
//    emit signal_change_gain_req(value);
    if(m_instance_re) m_instance_re->TriggerReqControlChange(RadarEngine::CT_GAIN,value);
}

void FrameControl2::on_lineEditGain_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    ui->horizontalSliderGain->setValue(value);
//    emit signal_change_gain_req(value);
    if(m_instance_re) m_instance_re->TriggerReqControlChange(RadarEngine::CT_GAIN,value);
}

void FrameControl2::on_pushButtonSea_clicked()
{
    if(ui->pushButtonSea->isChecked())
    {
        ui->pushButtonSea->setText(tr("Sea %1").arg("(Auto)"));
        ui->horizontalSliderSea->setEnabled(false);
        ui->lineEditSea->setEnabled(false);
//        emit signal_change_sea_req(-1);
        if(m_instance_re) m_instance_re->TriggerReqControlChange(RadarEngine::CT_SEA,-1);
    }
    else
    {
        ui->pushButtonSea->setText(tr("Sea %1").arg("(Man)"));
        ui->horizontalSliderSea->setEnabled(true);
        ui->lineEditSea->setEnabled(true);
        ui->lineEditSea->setText(QString::number(ui->horizontalSliderSea->value()));
    }
}
void FrameControl2::on_horizontalSliderSea_valueChanged(int value)
{
    ui->lineEditSea->setText(QString::number(value));
//    emit signal_change_sea_req(value);
    if(m_instance_re) m_instance_re->TriggerReqControlChange(RadarEngine::CT_SEA,value);
}

void FrameControl2::on_lineEditSea_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    ui->horizontalSliderSea->setValue(value);
//    emit signal_change_sea_req(value);
    if(m_instance_re) m_instance_re->TriggerReqControlChange(RadarEngine::CT_SEA,value);
}

void FrameControl2::on_horizontalSliderRain_valueChanged(int value)
{
    ui->lineEditRain->setText(QString::number(value));
//    emit signal_change_rain_req(value);
    if(m_instance_re) m_instance_re->TriggerReqControlChange(RadarEngine::CT_RAIN,value);
}

void FrameControl2::on_lineEditRain_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    ui->horizontalSliderRain->setValue(value);
//    emit signal_change_rain_req(value);
    if(m_instance_re) m_instance_re->TriggerReqControlChange(RadarEngine::CT_RAIN,value);
}

void FrameControl2::on_checkBoxMTI_clicked(bool checked)
{
    m_instance_cfg->setConfig(RadarEngine::VOLATILE_RADAR_PARAMS_FILTER_CONTROL_MTI,checked);
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
    m_instance_cfg->setConfig(RadarEngine::VOLATILE_RADAR_PARAMS_FILTER_DATA_MTI,mti_value);
    ui->horizontalSliderMTI->setValue(mti_value);
}

void FrameControl2::on_horizontalSliderMTI_valueChanged(int value)
{
    const int mti_value = value;
    m_instance_cfg->setConfig(RadarEngine::VOLATILE_RADAR_PARAMS_FILTER_DATA_MTI,mti_value);
//    mti_value = value;
    ui->lineEditMTI->setText(QString::number(mti_value));
}

void FrameControl2::on_checkBoxEBL_clicked(bool checked)
{
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_EBL_MARKER,checked);
}


void FrameControl2::on_checkBoxVRM_clicked(bool checked)
{
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_VRM_MARKER,checked);
}


void FrameControl2::on_horizontalSlideEBL_valueChanged(int value)
{
    const int ebl_value = value;
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_EBL_VALUE,ebl_value);
    ui->lineEditEBL->setText(QString::number(ebl_value));
}


void FrameControl2::on_horizontalSliderVRM_valueChanged(int value)
{
    const int vrm_value = value;
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_VRM_VALUE,vrm_value);
    ui->lineEditVRM->setText(QString::number(vrm_value));
}


void FrameControl2::on_lineEditEBL_textChanged(const QString &arg1)
{
    const int ebl_value = arg1.toInt();
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_EBL_VALUE,ebl_value);
    ui->horizontalSlideEBL->setValue(ebl_value);
}


void FrameControl2::on_lineEditVRM_textChanged(const QString &arg1)
{
    const int vrm_value = arg1.toInt();
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_VRM_VALUE,vrm_value);
    ui->horizontalSliderVRM->setValue(vrm_value);
}

