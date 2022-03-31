#ifndef FRAMECONTROL1_H
#define FRAMECONTROL1_H

#include <QFrame>

namespace Ui {
class FrameControl1;
}

class FrameControl1 : public QFrame
{
    Q_OBJECT

public:
    explicit FrameControl1(QWidget *parent = 0);
    ~FrameControl1();

    void setRangeRing(double ringWidth);
    void stateChange(int state);

signals:
    void signal_req_range(int range);
    void signal_req_Tx();
    void signal_req_Stby();
    void signal_req_shutdown();
    void signal_antena_man_switch();

private slots:
    void on_pushButtonZoomIn_clicked();

    void on_pushButtonZoomOut_clicked();

    void on_pushButtonTxStnb_clicked();

    void on_checkBoxShowRing_clicked(bool checked);

    void on_pushButtonTxStnb_2_clicked();

    void on_pushButtonRFToggle_clicked();

private:
    Ui::FrameControl1 *ui;
    QString ringValue;
};

#endif // FRAMECONTROL1_H
