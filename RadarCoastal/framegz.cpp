#include "framegz.h"
#include "radarconfig.h"
#include "ui_framegz.h"

#include <QIntValidator>

FrameGZ::FrameGZ(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameGZ)
{
    ui->setupUi(this);

    RadarConfig::RadarConfig* instance = RadarConfig::RadarConfig::getInstance("");
    int mode = instance->getConfig(RadarConfig::NON_VOLATILE_GZ_MODE).toInt();

    ui->checkBoxShowGZ->setChecked(instance->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool());
    ui->checkBoxAlarmGZ->setChecked(instance->getConfig(RadarConfig::NON_VOLATILE_GZ_ENABLE_ALARM).toBool());

    if(mode == 0) ui->radioButtonArc->setChecked(true);
    else ui->radioButtonCircle->setChecked(true);

    ui->lineEditInner->setValidator(new QIntValidator(0,100000,ui->lineEditInner));
    ui->lineEditOuter->setValidator(new QIntValidator(0,100000,ui->lineEditOuter));
    ui->lineEditStart->setValidator(new QIntValidator(0,360,ui->lineEditStart));
    ui->lineEditEnd->setValidator(new QIntValidator(0,360,ui->lineEditEnd));
    ui->lineEditNotifTHR->setValidator(new QIntValidator(10,500,ui->lineEditEnd));

    ui->lineEditInner->setText(instance->getConfig(RadarConfig::NON_VOLATILE_GZ_START_RANGE).toString());
    ui->lineEditOuter->setText(instance->getConfig(RadarConfig::NON_VOLATILE_GZ_END_RANGE).toString());
    ui->lineEditStart->setText(instance->getConfig(RadarConfig::NON_VOLATILE_GZ_START_BEARING).toString());
    ui->lineEditEnd->setText(instance->getConfig(RadarConfig::NON_VOLATILE_GZ_END_BEARING).toString());

    ui->lineEditNotifTHR->setText(instance->getConfig(RadarConfig::NON_VOLATILE_GZ_NOTIF_THRESHOLD).toString());
}

FrameGZ::~FrameGZ()
{
    delete ui;
}

void FrameGZ::on_checkBoxShowGZ_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ,checked);
}

void FrameGZ::on_checkBoxAlarmGZ_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_GZ_ENABLE_ALARM,checked);
}

void FrameGZ::on_radioButtonArc_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_GZ_MODE,static_cast<int>(!checked));
}

void FrameGZ::on_radioButtonCircle_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_GZ_MODE,static_cast<int>(checked));
}

void FrameGZ::on_lineEditInner_textChanged(const QString &arg1)
{
    bool ok;
    int range = arg1.toInt(&ok);
    if(ok) RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_GZ_START_RANGE,range);
}

void FrameGZ::on_lineEditOuter_textChanged(const QString &arg1)
{
    bool ok;
    int range = arg1.toInt(&ok);
    if(ok) RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_GZ_END_RANGE,range);
}

void FrameGZ::on_lineEditStart_textChanged(const QString &arg1)
{
    bool ok;
    int brn = arg1.toInt(&ok);
    if(ok) RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_GZ_START_BEARING,brn);
}

void FrameGZ::on_lineEditEnd_textChanged(const QString &arg1)
{
    bool ok;
    int brn = arg1.toInt(&ok);
    if(ok) RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_GZ_END_BEARING,brn);
}

void FrameGZ::on_lineEditNotifTHR_textChanged(const QString &arg1)
{
    bool ok;
    int notif = arg1.toInt(&ok);
    if(ok) RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_GZ_NOTIF_THRESHOLD,notif);
}

