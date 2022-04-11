#ifndef FRAMECONTROL3_H
#define FRAMECONTROL3_H

#include <QFrame>

namespace Ui {
class FrameControl3;
}

class FrameControl3 : public QFrame
{
    Q_OBJECT

public:
    explicit FrameControl3(QWidget *parent = nullptr);
    ~FrameControl3();

signals:
    void signal_PPIFullChanged();

private slots:
    void on_comboBoxMotion_currentIndexChanged(int index);

    void on_checkBoxShowCompass_clicked(bool checked);

    void on_checkBoxShowHM_clicked(bool checked);

private:
    Ui::FrameControl3 *ui;
};

#endif // FRAMECONTROL3_H
