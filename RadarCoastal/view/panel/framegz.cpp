#include "framegz.h"
#include "radarconfig.h"
#include "ui_framegz.h"

#include <QIntValidator>

FrameGZ::FrameGZ(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameGZ)
{
    ui->setupUi(this);

    RadarEngine::RadarConfig* instance = RadarEngine::RadarConfig::getInstance("");
    int mode = instance->getConfig(RadarEngine::NON_VOLATILE_GZ_MODE).toInt();
    int mode1 = instance->getConfig(RadarEngine::NON_VOLATILE_GZ_MODE1).toInt();

    ui->checkBoxShowGZ->setChecked(instance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool());
    ui->checkBoxAlarmGZ->setChecked(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_ENABLE_ALARM).toBool());
    ui->checkBoxShowGZ2->setChecked(instance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ1).toBool());
    ui->checkBoxAlarmGZ2->setChecked(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_ENABLE_ALARM1).toBool());

    if(mode == 0) ui->radioButtonArc->setChecked(true);
    else ui->radioButtonCircle->setChecked(true);
    if(mode1 == 0) ui->radioButtonArc2->setChecked(true);
    else ui->radioButtonCircle2->setChecked(true);

    ui->lineEditInner->setValidator(new QIntValidator(0,100000,ui->lineEditInner));
    ui->lineEditOuter->setValidator(new QIntValidator(0,100000,ui->lineEditOuter));
    ui->lineEditStart->setValidator(new QIntValidator(0,360,ui->lineEditStart));
    ui->lineEditEnd->setValidator(new QIntValidator(0,360,ui->lineEditEnd));
    ui->lineEditNotifTHR->setValidator(new QIntValidator(10,500,ui->lineEditNotifTHR));
    ui->lineEditInner2->setValidator(new QIntValidator(0,100000,ui->lineEditInner2));
    ui->lineEditOuter2->setValidator(new QIntValidator(0,100000,ui->lineEditOuter2));
    ui->lineEditStart2->setValidator(new QIntValidator(0,360,ui->lineEditStart2));
    ui->lineEditEnd2->setValidator(new QIntValidator(0,360,ui->lineEditEnd2));
    ui->lineEditNotifTHR2->setValidator(new QIntValidator(10,500,ui->lineEditNotifTHR2));

    ui->lineEditInner->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_START_RANGE).toString());
    ui->lineEditOuter->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_END_RANGE).toString());
    ui->lineEditStart->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_START_BEARING).toString());
    ui->lineEditEnd->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_END_BEARING).toString());
    ui->lineEditInner2->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_START_RANGE1).toString());
    ui->lineEditOuter2->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_END_RANGE1).toString());
    ui->lineEditStart2->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_START_BEARING1).toString());
    ui->lineEditEnd2->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_END_BEARING1).toString());

    ui->lineEditNotifTHR->setText(instance->getConfig(RadarEngine::NON_VOLATILE_GZ_NOTIF_THRESHOLD).toString());
}

FrameGZ::~FrameGZ()
{
    delete ui;
}

void FrameGZ::on_checkBoxShowGZ_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ,checked);
}

void FrameGZ::on_checkBoxAlarmGZ_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_ENABLE_ALARM,checked);
}

void FrameGZ::on_radioButtonArc_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_MODE,static_cast<int>(!checked));
}

void FrameGZ::on_radioButtonCircle_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_MODE,static_cast<int>(checked));
}

void FrameGZ::on_lineEditInner_textChanged(const QString &arg1)
{
    bool ok;
    int range = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_START_RANGE,range);
}

void FrameGZ::on_lineEditOuter_textChanged(const QString &arg1)
{
    bool ok;
    int range = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_END_RANGE,range);
}

void FrameGZ::on_lineEditStart_textChanged(const QString &arg1)
{
    bool ok;
    int brn = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_START_BEARING,brn);
}

void FrameGZ::on_lineEditEnd_textChanged(const QString &arg1)
{
    bool ok;
    int brn = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_END_BEARING,brn);
}

void FrameGZ::on_lineEditNotifTHR_textChanged(const QString &arg1)
{
    bool ok;
    int notif = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_NOTIF_THRESHOLD,notif);
}


void FrameGZ::on_checkBoxShowGZ2_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ1,checked);
}


void FrameGZ::on_checkBoxAlarmGZ2_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_ENABLE_ALARM1,checked);
}


void FrameGZ::on_radioButtonArc2_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_MODE1,static_cast<int>(!checked));
}


void FrameGZ::on_radioButtonCircle2_clicked(bool checked)
{
    RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_MODE1,static_cast<int>(checked));
}


void FrameGZ::on_lineEditInner2_textChanged(const QString &arg1)
{
    bool ok;
    int range = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_START_RANGE1,range);
}


void FrameGZ::on_lineEditOuter2_textChanged(const QString &arg1)
{
    bool ok;
    int range = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_END_RANGE1,range);
}


void FrameGZ::on_lineEditStart2_textChanged(const QString &arg1)
{
    bool ok;
    int brn = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_START_BEARING1,brn);
}


void FrameGZ::on_lineEditEnd2_textChanged(const QString &arg1)
{
    bool ok;
    int brn = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_END_BEARING1,brn);
}


void FrameGZ::on_lineEditNotifTHR2_textChanged(const QString &arg1)
{
    bool ok;
    int notif = arg1.toInt(&ok);
    if(ok) RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::NON_VOLATILE_GZ_NOTIF_THRESHOLD1,notif);
}

