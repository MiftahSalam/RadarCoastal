#include "trackmodelview.h"
#include "shared/utils.h"

TrackModelView::TrackModelView(QStandardItemModel *model): m_model{model}
{
}

void TrackModelView::UpdateModel(TrackModel trackModel)
{
    QList<QStandardItem *> listTarget = m_model->findItems(QString::number(trackModel.id),Qt::MatchExactly,0);
    if(!listTarget.isEmpty())
    {
        m_model->setData(m_model->index(m_model->rowCount(),0,QModelIndex()),QString::number(trackModel.id));
        m_model->setData(m_model->index(m_model->rowCount(),1,QModelIndex()),
                       QString::number(trackModel.rng,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),2,QModelIndex()),
                       QString::number(trackModel.brn,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),3,QModelIndex()),
                       QString::number(trackModel.spd,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),4,QModelIndex()),
                       QString::number(trackModel.crs,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),4,QModelIndex()),
                       QString::number(trackModel.lat,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),4,QModelIndex()),
                       QString::number(trackModel.lon,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),4,QModelIndex()),
                       QString::number(trackModel.alt,'f',Utils::TWO_PRECISION));
    }
}

void TrackModelView::InsertModel(TrackModel trackModel)
{
    QList<QStandardItem *> listTarget = m_model->findItems(QString::number(trackModel.id),Qt::MatchExactly,0);
    if(listTarget.isEmpty())
    {
        m_model->insertRow(m_model->rowCount(),QModelIndex());
        m_model->setData(m_model->index(m_model->rowCount(),0,QModelIndex()),QString::number(trackModel.id));
        m_model->setData(m_model->index(m_model->rowCount(),1,QModelIndex()),
                       QString::number(trackModel.rng,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),2,QModelIndex()),
                       QString::number(trackModel.brn,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),3,QModelIndex()),
                       QString::number(trackModel.spd,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),4,QModelIndex()),
                       QString::number(trackModel.crs,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),4,QModelIndex()),
                       QString::number(trackModel.lat,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),4,QModelIndex()),
                       QString::number(trackModel.lon,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount(),4,QModelIndex()),
                       QString::number(trackModel.alt,'f',Utils::TWO_PRECISION));

        m_model->item(m_model->rowCount()-1,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        m_model->item(m_model->rowCount()-1,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        m_model->item(m_model->rowCount()-1,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        m_model->item(m_model->rowCount()-1,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        m_model->item(m_model->rowCount()-1,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

void TrackModelView::Remove(const int &trackId)
{
    QList<QStandardItem *> listTarget = m_model->findItems(QString::number(trackId),Qt::MatchExactly,0);
    if(!listTarget.isEmpty())
    {
        int row = listTarget.at(0)->row();
        m_model->removeRow(row);
    }
}
