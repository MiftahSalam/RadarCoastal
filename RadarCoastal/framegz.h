#ifndef FRAMEGZ_H
#define FRAMEGZ_H

#include <QFrame>

namespace Ui {
class FrameGZ;
}

class FrameGZ : public QFrame
{
    Q_OBJECT

public:
    explicit FrameGZ(QWidget *parent = nullptr);
    ~FrameGZ() override;

private slots:
    void on_checkBoxShowGZ_clicked(bool checked);

    void on_checkBoxAlarmGZ_clicked(bool checked);

    void on_radioButtonArc_clicked(bool checked);

    void on_radioButtonCircle_clicked(bool checked);

    void on_lineEditInner_textChanged(const QString &arg1);

    void on_lineEditOuter_textChanged(const QString &arg1);

    void on_lineEditStart_textChanged(const QString &arg1);

    void on_lineEditEnd_textChanged(const QString &arg1);

    void on_lineEditNotifTHR_textChanged(const QString &arg1);

private:
    Ui::FrameGZ *ui;
};

#endif // FRAMEGZ_H
