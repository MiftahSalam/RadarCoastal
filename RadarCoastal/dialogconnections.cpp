#include "dialogconnections.h"
#include "ui_dialogconnections.h"

#include <radarconfig.h>

#include <QMessageBox>
#include <QIntValidator>
#include <QHostAddress>

DialogConnections::DialogConnections(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnections)
{
    ui->setupUi(this);

    RadarConfig::RadarConfig* rcInstance = RadarConfig::RadarConfig::getInstance("");

    ui->checkBoxShowARPA->setChecked(rcInstance->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_ARPA).toBool());

    ui->lineEditIPData->setValidator(new QIntValidator(0,255,ui->lineEditIPData));
    ui->lineEditPortData->setValidator(new QIntValidator(3000,65536,ui->lineEditPortData));
    ui->lineEditIPReport->setValidator(new QIntValidator(0,255,ui->lineEditIPReport));
    ui->lineEditPortReport->setValidator(new QIntValidator(3000,65536,ui->lineEditPortReport));
    ui->lineEditIPcmd->setValidator(new QIntValidator(0,255,ui->lineEditIPcmd));
    ui->lineEditPortcmd->setValidator(new QIntValidator(3000,65536,ui->lineEditPortcmd));

    ui->lineEditIPData->setText(rcInstance->getConfig(RadarConfig::NON_VOLATILE_RADAR_NET_IP_DATA).toString());
    ui->lineEditPortData->setText(QString::number(rcInstance->getConfig(RadarConfig::NON_VOLATILE_RADAR_NET_PORT_DATA).toInt()));
    ui->lineEditIPReport->setText(rcInstance->getConfig(RadarConfig::NON_VOLATILE_RADAR_NET_IP_REPORT).toString());
    ui->lineEditPortReport->setText(QString::number(rcInstance->getConfig(RadarConfig::NON_VOLATILE_RADAR_NET_PORT_REPORT).toInt()));
    ui->lineEditIPcmd->setText(rcInstance->getConfig(RadarConfig::NON_VOLATILE_RADAR_NET_IP_CMD).toString());
    ui->lineEditPortcmd->setText(QString::number(rcInstance->getConfig(RadarConfig::NON_VOLATILE_RADAR_NET_PORT_CMD).toInt()));


    QString arpa_conf = rcInstance->getConfig(RadarConfig::NON_VOLATILE_ARPA_NET_CONFIG).toString();
    QStringList arpa_conf_list = arpa_conf.split(";",Qt::SkipEmptyParts);

    if(arpa_conf_list.size() != 3)
    {
        ui->lineEditIPARPA->setText("invalid config "+arpa_conf);
        ui->lineEditPortARPA->setText("invalid config "+arpa_conf);
    }
    else
    {
        arpa_conf_list = arpa_conf_list.at(2).split(":",Qt::SkipEmptyParts);
        if(arpa_conf_list.size() != 3)
        {
            ui->lineEditIPARPA->setText("invalid config "+arpa_conf);
            ui->lineEditPortARPA->setText("invalid config "+arpa_conf);
        }
        else
        {
            ui->lineEditIPARPA->setText(arpa_conf_list.at(0)+":"+arpa_conf_list.at(1));
            ui->lineEditPortARPA->setText(arpa_conf_list.at(2));
        }
    }

}

DialogConnections::~DialogConnections()
{
    delete ui;
}

void DialogConnections::on_checkBoxShow_clicked(bool checked)
{
    RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_ARPA,checked);
}


void DialogConnections::on_pushButtonApply_clicked()
{
    RadarConfig::RadarConfig* rcInstance = RadarConfig::RadarConfig::getInstance("");

    rcInstance->setConfig(RadarConfig::NON_VOLATILE_RADAR_NET_IP_DATA,ui->lineEditIPData->text().remove(" "));
    rcInstance->setConfig(RadarConfig::NON_VOLATILE_RADAR_NET_PORT_DATA,ui->lineEditPortData->text().toUInt());
    rcInstance->setConfig(RadarConfig::NON_VOLATILE_RADAR_NET_IP_REPORT,ui->lineEditIPReport->text().remove(" "));
    rcInstance->setConfig(RadarConfig::NON_VOLATILE_RADAR_NET_PORT_REPORT,ui->lineEditPortReport->text().toUInt());
    rcInstance->setConfig(RadarConfig::NON_VOLATILE_RADAR_NET_IP_CMD,ui->lineEditIPcmd->text().remove(" "));
    rcInstance->setConfig(RadarConfig::NON_VOLATILE_RADAR_NET_PORT_CMD,ui->lineEditPortcmd->text().toUInt());

    QString arpa_conf = ui->lineEditIPARPA->text();
    QStringList arpa_conf_list = arpa_conf.split(":",Qt::SkipEmptyParts);

    if(arpa_conf_list.size() != 2)
    {
        ui->lineEditIPARPA->setText("");
        QMessageBox::information(this,"Warning","Invalid Mqtt server input.\n"
                                 "Input should be IP:port formatted");
        return;
    }
    else
    {
        QHostAddress host(arpa_conf_list.at(0));
        if(host.isNull())
        {
            ui->lineEditIPARPA->setText("");
            QMessageBox::information(this,"Warning","Invalid Mqtt server input host.\n"
                                     "Input should be IP formatted");
            return;
        }
        else
        {
            bool ok = false;
            uint port = arpa_conf_list.at(1).toUInt(&ok);
            Q_UNUSED(port)
            if(!ok)
            {
                ui->lineEditIPARPA->setText("");
                QMessageBox::information(this,"Warning","Invalid Mqtt server input port.\n"
                                         "Input port should be 2000-65500");
                return;
            }
            else
            {
                if(ui->lineEditPortARPA->text().isEmpty())
                {
                    ui->lineEditIPARPA->setText("");
                    QMessageBox::information(this,"Warning","Invalid Mqtt topic input.\n"
                                             "Input cannot be empty");
                    return;
                }
                else
                {
                    rcInstance->setConfig(RadarConfig::NON_VOLATILE_ARPA_NET_CONFIG,"mqtt;InOut;"+ui->lineEditIPARPA->text()+":"+ui->lineEditPortARPA->text());
                }
            }
        }
    }
}
