#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QResizeEvent>
#include <QMessageBox>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_re = new RadarEngine::RadarEngine(this);
    ui->frameControl1->setRadarEnginePtr(m_re);
    ui->frameControl2->setRadarEnginePtr(m_re);

    connect(ui->frameControl1,SIGNAL(signal_req_shutdown()),
            this,SLOT(trigger_shutdown()));

}

void MainWindow::trigger_shutdown()
{
    if(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_RADAR_STATUS).toInt() == RadarEngine::RADAR_TRANSMIT)
    {
        QMessageBox::information(this,"Warning","Radar is in Transmit Mode.\n"
                                 "Please turn to Standby Mode first");
        return;
    }

    m_re->trigger_stopRadar();
    sleep(1);
    close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    RadarConfig::RadarConfig *instance = RadarConfig::RadarConfig::getInstance("");
    if(!instance) qFatal("Cannot provide config service");
    instance->saveConfig();

    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    qDebug()<<Q_FUNC_INFO<<event->size()<<width()<<height();
    ui->frameControl1->move(10,0);
    ui->frameControl2->move(10,height()-ui->frameControl2->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}
