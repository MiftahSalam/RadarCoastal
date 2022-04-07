#ifndef FRAMETRACKDISPLAY_H
#define FRAMETRACKDISPLAY_H

#include <QFrame>
#include <QStandardItemModel>
#include <QTimer>
#include <QUdpSocket>

namespace Ui {
class FrameTrackDisplay;
}

class FrameTrackDisplay : public QFrame
{
    Q_OBJECT

public:
    explicit FrameTrackDisplay(QWidget *parent = 0);
    ~FrameTrackDisplay();

signals:
    void signal_request_del_track(bool r1,int id);

private slots:
    void trigger_target_update(
            bool r1,
            int id,
            double lat,
            double lon,
            double alt,
            double rng,
            double brn,
            double spd,
            double crs
            );
    void timerTimeout();

    void on_pushButtonDelSel_clicked();

    void on_pushButtonDelAll_clicked();

private:
    Ui::FrameTrackDisplay *ui;
    QTimer *timer;
    QUdpSocket *udpSocket;
    QString serverUdpIP;
    quint16 serverUdpPort;

    int dataCount_mqtt;
    QHash<int,quint64> target_time_tag_list,target_time_tag_list1;
    QStandardItemModel *model,*modelSend;

    void insertList(
            bool r1,
            int id,
            double lat,
            double lon,
            double alt,
            double rng,
            double brn,
            double spd,
            double crs
            );
};

#endif // FRAMETRACKDISPLAY_H
