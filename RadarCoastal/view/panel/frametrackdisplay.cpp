#include "frametrackdisplay.h"
#include "ui_frametrackdisplay.h"

#include <QDebug>
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

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTimeout()));

    model = new QStandardItemModel(this);
    model->setColumnCount(5);
    ui->tableViewTrack->setModel(model);

    modelSend = new QStandardItemModel(this);
    modelSend->setColumnCount(7);

    m_re = RadarEngine::RadarEngine::GetInstance();
    connect(m_re->radarArpa,&RadarEngine::RadarArpa::Signal_LostTarget,
            this,&FrameTrackDisplay::trigger_LostTarget);

    arpaSender = new ArpaSender(this);
    dataCount_mqtt = 0;
    updateCount = 0;
    cur_arpa_id_count = 0;

    timer->start(1000);
}

void FrameTrackDisplay::trigger_LostTarget(int id)
{
    qDebug()<<Q_FUNC_INFO<<id;
    removeTarget(id);
}

void FrameTrackDisplay::removeTarget(int id)
{
    QList<QStandardItem *> listTarget = model->findItems(QString::number(id),Qt::MatchExactly,0);
    if(!listTarget.isEmpty())
    {
        int row = listTarget.at(0)->row();
        model->removeRow(row);
        modelSend->removeRow(row);
    }
}

void FrameTrackDisplay::updateTarget()
{
    qDebug()<<Q_FUNC_INFO;
    if(m_re->radarArpa->targetNumber > 0)
    {
        RadarEngine::Position own_pos;
        own_pos.lat = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
        own_pos.lon = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
        Polar pol;
        double brn;
        double range;
        int num_limit = MAX_UPDATE_NUMBER;
        const bool heading_up = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool();
        const double currentHeading = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();
        double curRange = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
        //        const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

        //        switch (unit) {
        //        case 1:
        //            curRange *= KM_TO_NM;
        //            break;
        //        default:
        //            break;
        //        }

        while ((cur_arpa_id_count < m_re->radarArpa->targetNumber) && num_limit > 0)
        {
            if(m_re->radarArpa->targets[cur_arpa_id_count]->targetId > 0)
            {
                pol = Pos2Polar(m_re->radarArpa->targets[cur_arpa_id_count]->position,own_pos,curRange);
                brn = SCALE_RAW_TO_DEGREES2048(pol.angle);
                //                brn -= 270;
                brn = heading_up ? brn+currentHeading : brn;
                while(brn>360 || brn<0)
                {
                    if(brn>360)
                        brn -= 360;
                    if(brn<0)
                        brn += 360;
                }

                double arpa_course = m_re->radarArpa->targets[cur_arpa_id_count]->course;
                //                arpa_course -= 270;
                arpa_course = heading_up ? arpa_course+currentHeading : arpa_course;
                while(arpa_course>360 || arpa_course<0)
                {
                    if(arpa_course>360)
                        arpa_course -= 360;
                    if(arpa_course<0)
                        arpa_course += 360;
                }

                range = static_cast<double>(curRange*pol.r/RETURNS_PER_LINE)/1000.;
                /* untuk menghitung posisi yang sudah dikoreksi
                    pol.angle = SCALE_DEGREES_TO_RAW2048(brn);
                    Position arpa_pos = Polar2Pos(pol,own_pos,curRange);
                    */
                //                qDebug()<<Q_FUNC_INFO<<arpa->m_target[cur_arpa_id_count]->m_position.lat<<arpa->m_target[cur_arpa_id_count]->m_position.lon;
                trigger_target_update(
                            m_re->radarArpa->targets[cur_arpa_id_count]->targetId,
                            m_re->radarArpa->targets[cur_arpa_id_count]->position.lat,
                            m_re->radarArpa->targets[cur_arpa_id_count]->position.lon,
                            0.0,
                            range,
                            brn,
                            m_re->radarArpa->targets[cur_arpa_id_count]->speedKts,
                            arpa_course
                            );
                arpaSender->SendData(
                            m_re->radarArpa->targets[cur_arpa_id_count]->targetId,
                            m_re->radarArpa->targets[cur_arpa_id_count]->position.lat,
                            m_re->radarArpa->targets[cur_arpa_id_count]->position.lon,
                            0.0,
                            range,
                            brn,
                            m_re->radarArpa->targets[cur_arpa_id_count]->speedKts,
                            arpa_course
                            );
            }
            cur_arpa_id_count++;
            num_limit--;
        }
        if(cur_arpa_id_count >= m_re->radarArpa->targetNumber)
            cur_arpa_id_count = 0;
    }

}
void FrameTrackDisplay::timerTimeout()
{
    const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

    switch (unit) {
    case 0:
        ui->label_3->setText("Range\n(Km)");
        break;
    case 1:
        ui->label_3->setText("Range\n(NM)");
        break;
    default:
        break;
    }

    updateTarget();
    if(modelSend->rowCount()>0 && modelSend->rowCount()>dataCount_mqtt)
    {
        QString id,lat,lon,alt,spd,crs,mq_data,ttg;
        QModelIndex index = modelSend->index(dataCount_mqtt,0);
        QByteArray mq_databyte;

        id = modelSend->data(index).toString();
        index = modelSend->index(dataCount_mqtt,1);
        ttg = modelSend->data(index).toString();
        index = modelSend->index(dataCount_mqtt,2);
        lat = modelSend->data(index).toString();
        index = modelSend->index(dataCount_mqtt,3);
        lon = modelSend->data(index).toString();
        index = modelSend->index(dataCount_mqtt,4);
        alt = modelSend->data(index).toString();
        index = modelSend->index(dataCount_mqtt,5);
        spd = modelSend->data(index).toString();
        index = modelSend->index(dataCount_mqtt,6);
        crs = modelSend->data(index).toString();
        lat.replace(".",",");
        lon.replace(".",",");
        alt.replace(".",",");
        spd.replace(".",",");
        crs.replace(".",",");

        mq_data = "!"+id+"#"+ttg+"#"+lat+"#"+lon+"#"+alt+"#"+spd+"#"+crs+"@";
        //        mq_databyte = mq_data.toUtf8();
        //        udpSocket->writeDatagram(mq_databyte,QHostAddress::Broadcast,serverUdpPort);

        dataCount_mqtt++;
        //        qDebug()<<"mq_data"<<mq_data;
        //        qDebug()<<"Broadcast"<<serverUdpPort;
    }


    if(dataCount_mqtt==modelSend->rowCount())
    {
        //        qDebug()<<"dataCount_mqtt2"<<dataCount_mqtt;
        dataCount_mqtt = 0;
    }
    else if(dataCount_mqtt>modelSend->rowCount())
    {
        //        qDebug()<<"dataCount_mqtt3"<<dataCount_mqtt;
        dataCount_mqtt = modelSend->rowCount() - 1;
        if(dataCount_mqtt<1)
            dataCount_mqtt = 0;
    }
}

void FrameTrackDisplay::trigger_target_update(
        int id,
        double lat,
        double lon,
        double alt,
        double rng,
        double brn,
        double spd,
        double crs
        )
{
    const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());
    switch (unit) {
    case 1:
        rng *= KM_TO_NM;
        break;
    default:
        break;
    }

    QList<QStandardItem *> listTarget = model->findItems(QString::number(id),Qt::MatchExactly,0);
    if(!listTarget.isEmpty())
    {
        int row = listTarget.at(0)->row();
        model->setData(model->index(row,0,QModelIndex()),QString::number(id));
        model->setData(model->index(row,1,QModelIndex()),
                       QString::number(rng,'f',2));
        model->setData(model->index(row,2,QModelIndex()),
                       QString::number(brn,'f',2));
        model->setData(model->index(row,3,QModelIndex()),
                       QString::number(spd,'f',2));
        model->setData(model->index(row,4,QModelIndex()),
                       QString::number(crs,'f',2));

        modelSend->setData(modelSend->index(row,0,QModelIndex()),

                           QString::number(id));
        /*
            modelSend->setData(modelSend->index(row,1,QModelIndex()),
                           QString::number(QDateTime::currentMSecsSinceEpoch()));
            */
        modelSend->setData(modelSend->index(row,2,QModelIndex()),
                           QString::number(lat,'f',5));
        modelSend->setData(modelSend->index(row,3,QModelIndex()),
                           QString::number(lon,'f',5));
        modelSend->setData(modelSend->index(row,4,QModelIndex()),
                           QString::number(alt,'f',2));
        modelSend->setData(modelSend->index(row,5,QModelIndex()),
                           QString::number(spd,'f',2));
        modelSend->setData(modelSend->index(row,6,QModelIndex()),
                           QString::number(crs,'f',2));
    }
    else
        insertList(id, lat, lon, alt, rng, brn, spd, crs);
}
void FrameTrackDisplay::insertList(
        int id,
        double lat,
        double lon,
        double alt,
        double rng,
        double brn,
        double spd,
        double crs
        )
{
    model->insertRow(model->rowCount(),QModelIndex());
    modelSend->insertRow(modelSend->rowCount(),QModelIndex());

    model->setData(model->index(model->rowCount()-1,0,QModelIndex()),QString::number(id));
    model->setData(model->index(model->rowCount()-1,1,QModelIndex()),
                   QString::number(rng,'f',2));
    model->setData(model->index(model->rowCount()-1,2,QModelIndex()),
                   QString::number(brn,'f',2));
    model->setData(model->index(model->rowCount()-1,3,QModelIndex()),
                   QString::number(spd,'f',2));
    model->setData(model->index(model->rowCount()-1,4,QModelIndex()),
                   QString::number(crs,'f',2));

    modelSend->setData(modelSend->index(modelSend->rowCount()-1,0,QModelIndex()), QString::number(id));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,1,QModelIndex()),
                       QString::number(QDateTime::currentMSecsSinceEpoch()));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,2,QModelIndex()),
                       QString::number(lat,'f',5));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,3,QModelIndex()),
                       QString::number(lon,'f',5));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,4,QModelIndex()),
                       QString::number(alt,'f',2));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,5,QModelIndex()),
                       QString::number(spd,'f',2));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,6,QModelIndex()),
                       QString::number(crs,'f',2));

    model->item(model->rowCount()-1,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    model->item(model->rowCount()-1,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    model->item(model->rowCount()-1,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    model->item(model->rowCount()-1,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    model->item(model->rowCount()-1,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

FrameTrackDisplay::~FrameTrackDisplay()
{
    delete ui;
}

void FrameTrackDisplay::on_pushButtonDelSel_clicked()
{
    int row_count = model->rowCount();
    if(row_count>0)
    {
        int row = ui->tableViewTrack->currentIndex().row();
        QString id_str = model->index(row,0).data().toString();
        int id = id_str.toInt();

        for(int i=0;i<m_re->radarArpa->targetNumber;i++)
            if(m_re->radarArpa->targets[i]->targetId == id)
                m_re->radarArpa->targets[i]->SetStatusLost();
    }
}

void FrameTrackDisplay::on_pushButtonDelAll_clicked()
{
    int row_count = model->rowCount();
    if(row_count>0) m_re->radarArpa->DeleteAllTargets();
}
