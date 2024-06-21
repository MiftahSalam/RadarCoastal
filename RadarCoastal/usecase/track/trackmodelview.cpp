#include "trackmodelview.h"
#include "shared/utils.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, TrackModelView)
#else
#include <QDebug>
#endif

TrackModelView::TrackModelView(QStandardItemModel *model): m_model{model}
{
}

void TrackModelView::UpdateModel(TrackModel trackModel)
{
    QList<QStandardItem *> listTarget = m_model->findItems(QString::number(trackModel.id),Qt::MatchExactly,0);
    if(!listTarget.isEmpty())
    {
        int row = listTarget.at(0)->row();
        m_model->setData(m_model->index(row,0,QModelIndex()),QString::number(trackModel.id));
        m_model->setData(m_model->index(row,1,QModelIndex()),
                       QString::number(trackModel.rng,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(row,2,QModelIndex()),
                       QString::number(trackModel.brn,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(row,3,QModelIndex()),
                       QString::number(trackModel.spd,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(row,4,QModelIndex()),
                       QString::number(trackModel.crs,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(row,5,QModelIndex()),
                       QString::number(trackModel.lat,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(row,6,QModelIndex()),
                       QString::number(trackModel.lon,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(row,7,QModelIndex()),
                       QString::number(trackModel.alt,'f',Utils::TWO_PRECISION));
    }
}

void TrackModelView::InsertModel(TrackModel trackModel)
{
    QList<QStandardItem *> listTarget = m_model->findItems(QString::number(trackModel.id),Qt::MatchExactly,0);
    if(listTarget.isEmpty())
    {
        m_model->insertRow(m_model->rowCount(),QModelIndex());
        m_model->setData(m_model->index(m_model->rowCount()-1,0,QModelIndex()),QString::number(trackModel.id));
        m_model->setData(m_model->index(m_model->rowCount()-1,1,QModelIndex()),
                       QString::number(trackModel.rng,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount()-1,2,QModelIndex()),
                       QString::number(trackModel.brn,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount()-1,3,QModelIndex()),
                       QString::number(trackModel.spd,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount()-1,4,QModelIndex()),
                       QString::number(trackModel.crs,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount()-1,5,QModelIndex()),
                       QString::number(trackModel.lat,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount()-1,6,QModelIndex()),
                       QString::number(trackModel.lon,'f',Utils::TWO_PRECISION));
        m_model->setData(m_model->index(m_model->rowCount()-1,7,QModelIndex()),
                       QString::number(trackModel.alt,'f',Utils::TWO_PRECISION));

#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"model row count"<<m_model->rowCount();
#else
        qDebug()<<Q_FUNC_INFO<<"model row count"<<m_model->rowCount();
#endif

        /**/
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
