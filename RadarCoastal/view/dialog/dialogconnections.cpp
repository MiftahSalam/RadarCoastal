#include "dialogconnections.h"
#include "shared/utils.h"
#include "ui_dialogconnections.h"
#include "usecase/tx_timer/tx_timer_counter.h"

#include <QMessageBox>
#include <QIntValidator>
#include <QHostAddress>

DialogConnections::DialogConnections(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnections)
{    
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    appConfig = ApplicationConfig::getInstance();

    ui->setupUi(this);

    initConfig();

    auto elapsed = TxTimerCounter::GetCurrentElapsed();
    ui->lineEditOpTime->setText(Utils::TimeElapsedDisplay(elapsed));
}

void DialogConnections::initConfig()
{
    ui->checkBoxShowARPA->setChecked(appConfig->getPpiConfig()->getShowArpa());

    ui->lineEditIPData->setValidator(new QIntValidator(0,255,ui->lineEditIPData));
    ui->lineEditPortData->setValidator(new QIntValidator(3000,65536,ui->lineEditPortData));
    ui->lineEditIPReport->setValidator(new QIntValidator(0,255,ui->lineEditIPReport));
    ui->lineEditPortReport->setValidator(new QIntValidator(3000,65536,ui->lineEditPortReport));
    ui->lineEditIPcmd->setValidator(new QIntValidator(0,255,ui->lineEditIPcmd));
    ui->lineEditPortcmd->setValidator(new QIntValidator(3000,65536,ui->lineEditPortcmd));

    ui->lineEditIPData->setText(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_RADAR_NET_IP_DATA).toString());
    ui->lineEditPortData->setText(QString::number(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_RADAR_NET_PORT_DATA).toInt()));
    ui->lineEditIPReport->setText(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_RADAR_NET_IP_REPORT).toString());
    ui->lineEditPortReport->setText(QString::number(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_RADAR_NET_PORT_REPORT).toInt()));
    ui->lineEditIPcmd->setText(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_RADAR_NET_IP_CMD).toString());
    ui->lineEditPortcmd->setText(QString::number(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_RADAR_NET_PORT_CMD).toInt()));


    QString arpa_conf = appConfig->getArpaConfig()->getMqttInternal();
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList arpa_conf_list = arpa_conf.split(";", Qt::SkipEmptyParts);
#else
    QStringList arpa_conf_list = arpa_conf.split(";",QString::SkipEmptyParts);
#endif

    if(arpa_conf_list.size() != 3)
    {
        ui->lineEditIPARPA->setText("invalid config "+arpa_conf);
        ui->lineEditPortARPA->setText("invalid config "+arpa_conf);
    }
    else
    {
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
        arpa_conf_list = arpa_conf_list.at(2).split(":", Qt::SkipEmptyParts);
#else
        arpa_conf_list = arpa_conf_list.at(2).split(":",QString::SkipEmptyParts);
#endif
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

    QString nav_conf = appConfig->getNavConfig()->getMqttInternal();
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList nav_conf_list = nav_conf.split(";", Qt::SkipEmptyParts);
#else
    QStringList nav_conf_list = nav_conf.split(";",QString::SkipEmptyParts);
#endif

    if(nav_conf_list.size() != 3)
    {
        ui->lineEditNavMqttServer->setText("invalid config "+nav_conf);
        ui->lineEditNavMqttTopic->setText("invalid config "+nav_conf);
    }
    else
    {
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
        nav_conf_list = nav_conf_list.at(2).split(":", Qt::SkipEmptyParts);
#else
        nav_conf_list = nav_conf_list.at(2).split(":",QString::SkipEmptyParts);
#endif
        if(nav_conf_list.size() != 3)
        {
            ui->lineEditNavMqttServer->setText("invalid config "+nav_conf);
            ui->lineEditNavMqttTopic->setText("invalid config "+nav_conf);
        }
        else
        {
            ui->lineEditNavMqttServer->setText(nav_conf_list.at(0)+":"+nav_conf_list.at(1));
            ui->lineEditNavMqttTopic->setText(nav_conf_list.at(2));
        }
    }
}

DialogConnections::~DialogConnections()
{
    delete ui;
}

void DialogConnections::upateOpTIme(QString elapsed)
{
    ui->lineEditOpTime->setText(elapsed);
}

void DialogConnections::on_checkBoxShow_clicked(bool checked)
{
    appConfig->getPpiConfig()->setShowArpa(checked);
}


void DialogConnections::on_pushButtonApply_clicked()
{
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_RADAR_NET_IP_DATA,ui->lineEditIPData->text().remove(" "));
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_RADAR_NET_PORT_DATA,ui->lineEditPortData->text().toUInt());
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_RADAR_NET_IP_REPORT,ui->lineEditIPReport->text().remove(" "));
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_RADAR_NET_PORT_REPORT,ui->lineEditPortReport->text().toUInt());
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_RADAR_NET_IP_CMD,ui->lineEditIPcmd->text().remove(" "));
    m_instance_cfg->setConfig(RadarEngine::NON_VOLATILE_RADAR_NET_PORT_CMD,ui->lineEditPortcmd->text().toUInt());

    QString arpa_conf = ui->lineEditIPARPA->text();
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList arpa_conf_list = arpa_conf.split(":", Qt::SkipEmptyParts);
#else
    QStringList arpa_conf_list = arpa_conf.split(":",QString::SkipEmptyParts);
#endif

    if(arpa_conf_list.size() != 2)
    {
        ui->lineEditIPARPA->setText("");
        QMessageBox::information(this,tr("Warning"),tr("Invalid Mqtt server input.")+"\n"+tr("Input should be IP:port formatted"));
        return;
    }
    else
    {
        QHostAddress host(arpa_conf_list.at(0));
        if(host.isNull())
        {
            ui->lineEditIPARPA->setText("");
            QMessageBox::information(this,tr("Warning"),tr("Invalid Mqtt server input host.")+"\n"+tr("Input should be IP formatted"));
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
                QMessageBox::information(this,tr("Warning"),tr("Invalid Mqtt server input port.")+"\n"+tr("Input port should be 2000-65500"));
                return;
            }
            else
            {
                if(ui->lineEditPortARPA->text().isEmpty())
                {
                    ui->lineEditIPARPA->setText("");
                    QMessageBox::information(this,tr("Warning"),tr("Invalid Mqtt topic input.")+"\n"+tr("Input cannot be empty"));
                    return;
                }
                else
                {
                    appConfig->getArpaConfig()->setMqttInternal("mqtt;InOut;"+ui->lineEditIPARPA->text()+":"+ui->lineEditPortARPA->text());
                }
            }
        }
    }
}

void DialogConnections::on_pushButtonApplyNav_clicked()
{
    QString nav_conf = ui->lineEditNavMqttServer->text();
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    QStringList nav_conf_list = nav_conf.split(":", Qt::SkipEmptyParts);
#else
    QStringList nav_conf_list = nav_conf.split(":",QString::SkipEmptyParts);
#endif

    if(nav_conf_list.size() != 2)
    {
        ui->lineEditNavMqttServer->setText("");
        QMessageBox::information(this,tr("Warning"),tr("Invalid Mqtt server input.")+"\n"+tr("Input should be IP:port formatted"));
        return;
    }
    else
    {
        QHostAddress host(nav_conf_list.at(0));
        if(host.isNull())
        {
            ui->lineEditNavMqttServer->setText("");
            QMessageBox::information(this,tr("Warning"),tr("Invalid Mqtt server input host.")+"\n"+tr("Input should be IP formatted"));
            return;
        }
        else
        {
            bool ok = false;
            uint port = nav_conf_list.at(1).toUInt(&ok);
            Q_UNUSED(port)
            if(!ok)
            {
                ui->lineEditNavMqttServer->setText("");
                QMessageBox::information(this,tr("Warning"),tr("Invalid Mqtt server input port.")+"\n"+tr("Input port should be 2000-65500"));
                return;
            }
            else
            {
                if(ui->lineEditNavMqttTopic->text().isEmpty())
                {
                    ui->lineEditNavMqttTopic->setText("");
                    QMessageBox::information(this,tr("Warning"),tr("Invalid Mqtt topic input.")+"\n"+tr("Input cannot be empty"));
                    return;
                }
                else
                {
                    appConfig->getNavConfig()->setMqttInternal("mqtt;InOut;"+ui->lineEditNavMqttServer->text()+":"+ui->lineEditNavMqttTopic->text());
                }
            }
        }
    }

}

