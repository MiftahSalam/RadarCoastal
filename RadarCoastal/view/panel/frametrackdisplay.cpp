#include "frametrackdisplay.h"
#include "ui_frametrackdisplay.h"

#include <QDateTime>
#include <QMenu>
#include <QSettings>
#include <QDir>


#define MAX_UPDATE_NUMBER 5

FrameTrackDisplay::FrameTrackDisplay(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameTrackDisplay)
{
    ui->setupUi(this);
    ui->label_3->setText(tr("Range%1%2").arg("\n").arg("(km)"));
    ui->label_4->setText(tr("Bearing%1%2").arg("\n").arg("(deg)"));
    ui->label_5->setText(tr("Speed%1%2").arg("\n").arg("(kts)"));
    ui->label_6->setText(tr("Course%1%2").arg("\n").arg("(deg)"));

    track = Track::GetInstance();

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTimeout()));

    ui->tableViewTrack->setModel(track->GetModelView());

    timer->start(1000);
}

void FrameTrackDisplay::timerTimeout()
{
    const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

    switch (unit) {
    case 0:
        ui->label_3->setText(tr("Range%1%2").arg("\n").arg("(km)"));
        break;
    case 1:
        ui->label_3->setText(tr("Range%1%2").arg("\n").arg("(NM)"));
        break;
    default:
        break;
    }

}

FrameTrackDisplay::~FrameTrackDisplay()
{
    delete ui;
}

void FrameTrackDisplay::on_pushButtonDelSel_clicked()
{
    int row_count = ui->tableViewTrack->model()->rowCount();
    if(row_count>0)
    {
        int row = ui->tableViewTrack->currentIndex().row();
        QString id_str = ui->tableViewTrack->model()->index(row,0).data().toString();

        track->RemoveTrack(id_str);
    }
}

void FrameTrackDisplay::on_pushButtonDelAll_clicked()
{
    int row_count =  ui->tableViewTrack->model()->rowCount();
    if(row_count>0) track->RemoveAllTrack();
}
