#include "frametrackdisplay.h"
#include "ui_frametrackdisplay.h"

#include <QDebug>
#include <QDateTime>
#include <QMenu>
#include <QSettings>
#include <QDir>

FrameTrackDisplay::FrameTrackDisplay(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameTrackDisplay)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTimeout()));

    model = new QStandardItemModel(this);
    model->setColumnCount(5);

    modelSend = new QStandardItemModel(this);
    modelSend->setColumnCount(7);

    dataCount_mqtt = 0;

    QSettings config(QDir::homePath()+"/.armed20/radar.conf",QSettings::IniFormat);

    serverUdpIP = config.value("server/ip","127.0.0.1").toString();
    serverUdpPort = config.value("server/port",1883).toUInt();
    qDebug()<<Q_FUNC_INFO<<serverUdpIP<<serverUdpPort;
//    serverUdpPort = 45454;

    udpSocket = new QUdpSocket(this);

    ui->tableViewTrack->setModel(model);

    timer->start(1000);
}

void FrameTrackDisplay::timerTimeout()
{
    QHashIterator<int,quint64> i(target_time_tag_list);
    QList<int> target_to_delete;
    quint64 now = QDateTime::currentMSecsSinceEpoch();

    target_to_delete.clear();
    while(i.hasNext())
    {
        i.next();
        if(now-i.value()>1000)
            target_to_delete.append(i.key());
    }

    for(int i=0;i<target_to_delete.size();i++)
    {
        target_time_tag_list.remove(target_to_delete.at(i));

        QList<QStandardItem *> listTarget = model->findItems("R1-"+QString::number(target_to_delete.at(i)),0);
        if(!listTarget.isEmpty())
        {
            int row = listTarget.at(0)->row();
            model->removeRow(row);
            modelSend->removeRow(row);
        }
    }

    QHashIterator<int,quint64> j(target_time_tag_list1);

    target_to_delete.clear();
    while(j.hasNext())
    {
        j.next();
        if(now-j.value()>1000)
            target_to_delete.append(j.key());
    }

    for(int i=0;i<target_to_delete.size();i++)
    {
        target_time_tag_list1.remove(target_to_delete.at(i));

        QList<QStandardItem *> listTarget = model->findItems("R2-"+QString::number(target_to_delete.at(i)),0);
        if(!listTarget.isEmpty())
        {
            int row = listTarget.at(0)->row();
            model->removeRow(row);
            modelSend->removeRow(row);
        }
    }

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
        mq_databyte = mq_data.toUtf8();
        udpSocket->writeDatagram(mq_databyte,QHostAddress::Broadcast,serverUdpPort);

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
        bool r1,
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
    spd *= 1.852;
    quint64 new_target_tt;
    QHash<int,quint64> *cur_target_time_tag_list = r1 ? &target_time_tag_list : &target_time_tag_list1;
    if(cur_target_time_tag_list->contains(id))
    {
        new_target_tt = cur_target_time_tag_list->value(id);
        new_target_tt = QDateTime::currentMSecsSinceEpoch();
        cur_target_time_tag_list->remove(id);
        cur_target_time_tag_list->insert(id,new_target_tt);

        QList<QStandardItem *> listTarget = model->findItems(r1 ? "R1-"+QString::number(id) : "R2-"+QString::number(id)
                                                                  ,0);
        if(!listTarget.isEmpty())
        {
            int row = listTarget.at(0)->row();
            model->setData(model->index(row,0,QModelIndex()),
                           r1 ? "R1-"+QString::number(id) : "R2-"+QString::number(id));
            model->setData(model->index(row,1,QModelIndex()),
                           QString::number(rng,'f',1));
            model->setData(model->index(row,2,QModelIndex()),
                           QString::number(brn,'f',1));
            model->setData(model->index(row,3,QModelIndex()),
                           QString::number(spd,'f',1));
            model->setData(model->index(row,4,QModelIndex()),
                           QString::number(crs,'f',1));

            modelSend->setData(modelSend->index(row,0,QModelIndex()),
                           r1 ? "R1-"+QString::number(id) : "R2-"+QString::number(id));
            /*
            modelSend->setData(modelSend->index(row,1,QModelIndex()),
                           QString::number(QDateTime::currentMSecsSinceEpoch()));
            */
            modelSend->setData(modelSend->index(row,2,QModelIndex()),
                           QString::number(lat,'f',5));
            modelSend->setData(modelSend->index(row,3,QModelIndex()),
                           QString::number(lon,'f',5));
            modelSend->setData(modelSend->index(row,4,QModelIndex()),
                           QString::number(alt,'f',1));
            modelSend->setData(modelSend->index(row,5,QModelIndex()),
                           QString::number(spd,'f',1));
            modelSend->setData(modelSend->index(row,6,QModelIndex()),
                           QString::number(crs,'f',1));
        }
        else
            insertList(r1,id, lat, lon, alt, rng, brn, spd, crs);
    }
    else
    {
        cur_target_time_tag_list->insert(id,QDateTime::currentMSecsSinceEpoch());
        insertList(r1,id, lat, lon, alt, rng, brn, spd, crs);
    }
}
void FrameTrackDisplay::insertList(
        bool r1,
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

    model->setData(model->index(model->rowCount()-1,0,QModelIndex()),
                   r1 ? "R1-"+QString::number(id) : "R2-"+QString::number(id));
    model->setData(model->index(model->rowCount()-1,1,QModelIndex()),
                   QString::number(rng,'f',1));
    model->setData(model->index(model->rowCount()-1,2,QModelIndex()),
                   QString::number(brn,'f',1));
    model->setData(model->index(model->rowCount()-1,3,QModelIndex()),
                   QString::number(spd,'f',1));
    model->setData(model->index(model->rowCount()-1,4,QModelIndex()),
                   QString::number(crs,'f',1));

    modelSend->setData(modelSend->index(modelSend->rowCount()-1,0,QModelIndex()),
                   r1 ? "R1-"+QString::number(id) : "R2-"+QString::number(id));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,1,QModelIndex()),
                   QString::number(QDateTime::currentMSecsSinceEpoch()));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,2,QModelIndex()),
                   QString::number(lat,'f',5));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,3,QModelIndex()),
                   QString::number(lon,'f',5));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,4,QModelIndex()),
                   QString::number(alt,'f',1));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,5,QModelIndex()),
                   QString::number(spd,'f',1));
    modelSend->setData(modelSend->index(modelSend->rowCount()-1,6,QModelIndex()),
                   QString::number(crs,'f',1));

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
        bool r1 = id_str.contains("R1");
        int id = id_str.remove(0,3).toInt();

        emit signal_request_del_track(r1,id);
    }
}

void FrameTrackDisplay::on_pushButtonDelAll_clicked()
{
    int row_count = model->rowCount();
    if(row_count>0)
        emit signal_request_del_track(true,-100);
}
