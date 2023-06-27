#include "trackrepository_mem.h"

#include "shared/utils.h"

TrackRepository* TrackRepository::Create()
{
    return new TrackRepository_Mem();
}

TrackRepository_Mem::TrackRepository_Mem()
{
}

void TrackRepository_Mem::Insert(const TrackModel &trackModel)
{
    QString key = QString::number(trackModel.id);
    TrackModel *track = model.value(key);
    if (track == nullptr) {
        model.insert(key,new TrackModel(trackModel));
    }
//    QList<QStandardItem *> listTarget = model->findItems(QString::number(trackModel.id),Qt::MatchExactly,0);
//    if(listTarget.isEmpty())
//    {
//        model->insertRow(model->rowCount(),QModelIndex());
//        model->setData(model->index(model->rowCount(),0,QModelIndex()),QString::number(trackModel.id));
//        model->setData(model->index(model->rowCount(),1,QModelIndex()),
//                       QString::number(trackModel.rng,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(model->rowCount(),2,QModelIndex()),
//                       QString::number(trackModel.brn,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(model->rowCount(),3,QModelIndex()),
//                       QString::number(trackModel.spd,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(model->rowCount(),4,QModelIndex()),
//                       QString::number(trackModel.crs,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(model->rowCount(),4,QModelIndex()),
//                       QString::number(trackModel.lat,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(model->rowCount(),4,QModelIndex()),
//                       QString::number(trackModel.lon,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(model->rowCount(),4,QModelIndex()),
//                       QString::number(trackModel.alt,'f',Utils::TWO_PRECISION));

//        model->item(model->rowCount()-1,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//        model->item(model->rowCount()-1,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//        model->item(model->rowCount()-1,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//        model->item(model->rowCount()-1,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//        model->item(model->rowCount()-1,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//    }
}

void TrackRepository_Mem::Update(const TrackModel &trackModel)
{
    QString key = QString::number(trackModel.id);
    TrackModel *track = model.value(key);
    if (track) {
        model.insert(key,new TrackModel(trackModel));
    }

//    QList<QStandardItem *> listTarget = model->findItems(QString::number(trackModel.id),Qt::MatchExactly,0);
//    if(!listTarget.isEmpty())
//    {
//        int row = listTarget.at(0)->row();
//        model->setData(model->index(row,0,QModelIndex()),QString::number(trackModel.id));
//        model->setData(model->index(row,1,QModelIndex()),
//                       QString::number(trackModel.rng,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(row,2,QModelIndex()),
//                       QString::number(trackModel.brn,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(row,3,QModelIndex()),
//                       QString::number(trackModel.spd,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(row,4,QModelIndex()),
//                       QString::number(trackModel.crs,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(row,4,QModelIndex()),
//                       QString::number(trackModel.lat,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(row,4,QModelIndex()),
//                       QString::number(trackModel.lon,'f',Utils::TWO_PRECISION));
//        model->setData(model->index(row,4,QModelIndex()),
//                       QString::number(trackModel.alt,'f',Utils::TWO_PRECISION));
//    }

}

std::list<TrackModel*> TrackRepository_Mem::FindAll()
{
//    QList<QStandardItem *> listTarget = model->findItems(QString::number(trackId),Qt::MatchExactly,0);
    QList<TrackModel*> listTarget = model.values();
    return listTarget.toStdList();
}

TrackModel* TrackRepository_Mem::FindOne(const int &trackId)
{
//    QList<QStandardItem *> listTarget = model->findItems(QString::number(trackId),Qt::MatchExactly,0);
    QString key = QString::number(trackId);
    TrackModel* target = model.value(key);

    return target;
}

void TrackRepository_Mem::Remove(const int &trackId)
{
//    QList<QStandardItem *> listTarget = model->findItems(QString::number(trackId),Qt::MatchExactly,0);
//    if(!listTarget.isEmpty())
//    {
//        int row = listTarget.at(0)->row();
//        model->removeRow(row);
//    }
    model.remove(QString::number(trackId));
}
