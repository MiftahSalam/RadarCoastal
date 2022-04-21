#include "dialogradar.h"
#include "ui_dialogradar.h"
#include "radar/radar_global.h"

#include <QMessageBox>

DialogRadar::DialogRadar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRadar)
{
    ui->setupUi(this);
    ui->checkBoxShowARPA->setChecked(arpa_settings.show);

    ui->lineEditIPData->setValidator(new QIntValidator(0,255,ui->lineEditIPData));
    ui->lineEditPortData->setValidator(new QIntValidator(3000,65536,ui->lineEditPortData));
    ui->lineEditIPReport->setValidator(new QIntValidator(0,255,ui->lineEditIPReport));
    ui->lineEditPortReport->setValidator(new QIntValidator(3000,65536,ui->lineEditPortReport));
    ui->lineEditIPcmd->setValidator(new QIntValidator(0,255,ui->lineEditIPcmd));
    ui->lineEditPortcmd->setValidator(new QIntValidator(3000,65536,ui->lineEditPortcmd));
    ui->lineEditPanPosition->setValidator(new QDoubleValidator(0,360,2,ui->lineEditPanPosition));
    ui->lineEditPanPosition->setValidator(new QDoubleValidator(-90,90,2,ui->lineEditTiltPosition));

    ui->lineEditIPData->setText(radar_settings.ip_data);
    ui->lineEditPortData->setText(QString::number(radar_settings.port_data));
    ui->lineEditIPReport->setText(radar_settings.ip_report);
    ui->lineEditPortReport->setText(QString::number(radar_settings.port_report));
    ui->lineEditIPcmd->setText(radar_settings.ip_command);
    ui->lineEditPortcmd->setText(QString::number(radar_settings.port_command));
    ui->lineEditIPARPA->setText(arpa_settings.ip);
    ui->lineEditPortARPA->setText(QString::number(arpa_settings.port));
    ui->lineEditPanPosition->setText("0");
    ui->lineEditTiltPosition->setText("0");

}

DialogRadar::~DialogRadar()
{
    delete ui;
}

void DialogRadar::on_checkBoxShow_clicked(bool checked)
{
    arpa_settings.show = checked;
}


void DialogRadar::on_pushButtonApply_clicked()
{
    radar_settings.ip_data = ui->lineEditIPData->text().remove(" ");
    radar_settings.port_data = ui->lineEditPortData->text().toUInt();
    radar_settings.ip_report = ui->lineEditIPReport->text().remove(" ");
    radar_settings.port_report = ui->lineEditPortReport->text().toUInt();
    radar_settings.ip_command = ui->lineEditIPcmd->text().remove(" ");
    radar_settings.port_command = ui->lineEditPortcmd->text().toUInt();
    arpa_settings.ip = ui->lineEditIPARPA->text().remove(" ");
    arpa_settings.port = ui->lineEditPortARPA->text().toUInt();

    emit signal_settingChange();
}


void DialogRadar::on_pushButtonApplyPos_clicked()
{
    double pan_pos,tilt_pos;
    bool valid;

    pan_pos = ui->lineEditPanPosition->text().toDouble(&valid);
    if(!valid)
    {
        QMessageBox::information(this,"Warning","Invalid pan position input.\n"
                                 "Input should be in range -180 to 180");
        return;
    }

    tilt_pos = ui->lineEditTiltPosition->text().toDouble(&valid);
    if(!valid)
    {
        QMessageBox::information(this,"Warning","Invalid tilt position input.\n"
                                 "Input should be in range -90 to 90");
        return;
    }

    emit signal_panTiltPositionEdited(pan_pos,tilt_pos);

}
