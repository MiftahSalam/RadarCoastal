#include "shared/utils.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QResizeEvent>
#include <QMessageBox>
#include <unistd.h>

const int PADDING = 5;
const int MARGIN = 10;
const int MAX_HEIGHT_THRESHOLD = 900;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Utils::unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());
    m_re = RadarEngine::RadarEngine::GetInstance(this);
    m_cfg = RadarEngine::RadarConfig::getInstance("");

    ui->setupUi(this);
    ui->retranslateUi(this);
    ui->pushButtonTilting->hide();

    setWindowTitle("Coastal Radar");

    m_ppi = new RadarWidget(centralWidget());
    m_dialog_conns = new DialogConnections(this);
    m_dialog_gz = new DialogGZ(this);
    m_dialog_bit = new DialogBIT(this);

    connect(ui->frameControl1,SIGNAL(signal_req_shutdown()),this,SLOT(TriggerShutdown()));

    connect(ui->pushButtonConnections, &QAbstractButton::clicked, this, &MainWindow::OnPushButtonConnectionsClicked);
    connect(ui->pushButtonBIT, &QAbstractButton::clicked, this, &MainWindow::OnPushButtonBITClicked);
    connect(ui->pushButtonSetGZ, &QAbstractButton::clicked, this, &MainWindow::OnPushButtonSetGZClicked);

    connect(m_re,&RadarEngine::RadarEngine::SignalPlotRadarSpoke,m_ppi,&RadarWidget::trigger_DrawSpoke);
    connect(m_ppi,&RadarWidget::signal_cursorMove,ui->frameCursor,&FrameCursor::trigger_cursorMove);

}

void MainWindow::setupPPILayout()
{
    int radarWidgetWidth = width()-ui->frameRight->width()-ui->frameLeft->width();
    int radarWidgetHeight = height();
    int side = qMin(radarWidgetWidth, radarWidgetHeight);
    int radarWidgetX = (radarWidgetWidth/2)-(side/2);
    int radarWidgetY = (height()/2)-(side/2);

    m_ppi->clearMask();

    /*
    */
    QRect rect;
    m_ppi->setGeometry(radarWidgetX,radarWidgetY,side,side);
    rect = QRect(PADDING,PADDING,m_ppi->width()-MARGIN,m_ppi->height()-MARGIN);
    QRegion region = QRegion(rect,QRegion::Ellipse);
    m_ppi->setRectRegoin(rect);
    m_ppi->setMask(region);

}

void MainWindow::TriggerShutdown()
{
    if(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::VOLATILE_RADAR_STATUS).toInt() == RadarEngine::RADAR_TRANSMIT)
    {
        QMessageBox::information(this, tr("Warning"),tr("Radar is in Transmit Mode")+".\n"+
                                 tr("Please turn to Standby Mode first"));
        return;
    }

    m_re->TriggerStopRadar();
    sleep(1);
    close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    RadarEngine::RadarConfig *instance = RadarEngine::RadarConfig::getInstance("");
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
    if(height() > MAX_HEIGHT_THRESHOLD)
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

void MainWindow::OnPushButtonBITClicked()
{
    m_dialog_bit->show();
}

void MainWindow::OnPushButtonConnectionsClicked()
{
    m_dialog_conns->show();
}

void MainWindow::OnPushButtonSetGZClicked()
{
    m_dialog_gz->show();
}
