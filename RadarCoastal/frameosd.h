#ifndef FRAMEOSD_H
#define FRAMEOSD_H

#include <QFrame>

#include "stream/navsensor.h"

namespace Ui {
class FrameOSD;
}

class FrameOSD : public QFrame
{
    Q_OBJECT

public:
    explicit FrameOSD(QWidget *parent = nullptr);
    ~FrameOSD();

private slots:
    void on_timeout();

    void on_pushButtonApply_clicked();

private:
    Ui::FrameOSD *ui;
    NavSensor* sensor;

    quint8 no_hdg_count,no_gps_count;
    bool hdg_col_normal,gps_col_normal;

};

#endif // FRAMEOSD_H
