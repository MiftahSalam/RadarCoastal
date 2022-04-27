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
    ui->pushButtonTilting->hide();

    setWindowTitle("Coastal Radar");

    m_re = RadarEngine::RadarEngine::getInstance(this);
    ppi = new RadarWidget(centralWidget());
    dConns = new DialogConnections(this);
    dGZ = new DialogGZ(this);

    connect(ui->frameControl1,SIGNAL(signal_req_shutdown()),
            this,SLOT(trigger_shutdown()));
    connect(m_re,&RadarEngine::RadarEngine::signal_plotRadarSpoke,ppi,&RadarWidget::trigger_DrawSpoke);
    connect(ppi,&RadarWidget::signal_cursorMove,ui->frameCursor,&FrameCursor::trigger_cursorMove);

}

void MainWindow::setupPPILayout()
{
    int radarWidgetWidth = width()-ui->frameRight->width()-ui->frameLeft->width();
    int radarWidgetHeight = height();
    int side = qMin(radarWidgetWidth, radarWidgetHeight);
    int radarWidgetX = (radarWidgetWidth/2)-(side/2);
    int radarWidgetY = (height()/2)-(side/2);

    ppi->clearMask();

    /*
    */
    QRect rect;
    ppi->setGeometry(radarWidgetX,radarWidgetY,side,side);
    rect = QRect(5,5,ppi->width()-10,ppi->height()-10);
    QRegion region = QRegion(rect,QRegion::Ellipse);
    ppi->setRectRegoin(rect);
    ppi->setMask(region);

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

    ui->frameRight->move(width()-ui->frameRight->width(),0);
    ui->frameRight->resize(ui->frameRight->width(),height());
    ui->frameLeft->move(width()-ui->frameRight->width()-ui->frameLeft->width(),0);
    ui->frameLeft->resize(ui->frameLeft->width(),height());
    ui->frameCursor->move(width()-ui->frameRight->width()-ui->frameLeft->width()-ui->frameCursor->width(),height()-ui->frameCursor->height());
    ui->frameCursor->resize(ui->frameCursor->width(),ui->frameCursor->height());
    if(height() > 900)
    {
        ui->frameStatus->move(width()-ui->frameRight->width()-ui->frameLeft->width()-ui->frameStatus->width(),height()-ui->frameStatus->height()-ui->frameCursor->height());
        ui->pushButtonSetGZ->hide();
        ui->frameGZ->show();
    }
    else
    {
        ui->frameStatus->move(width()-ui->frameRight->width()-ui->frameLeft->width()-ui->frameStatus->width(),0);
        ui->pushButtonSetGZ->show();
        ui->frameGZ->hide();
    }
    ui->frameStatus->resize(ui->frameStatus->width(),ui->frameStatus->height());

    setupPPILayout();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonBIT_clicked()
{
}

void MainWindow::on_pushButtonConnections_clicked()
{
    dConns->show();
}

void MainWindow::on_pushButtonSetGZ_clicked()
{
    dGZ->show();
}

