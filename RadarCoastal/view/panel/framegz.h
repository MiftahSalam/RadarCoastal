#ifndef FRAMEGZ_H
#define FRAMEGZ_H

#include "shared/config/gz_config.h"
#include <QFrame>

#include <RadarEngine/radarconfig.h>

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

    void on_checkBoxShowGZ2_clicked(bool checked);

    void on_checkBoxAlarmGZ2_clicked(bool checked);

    void on_radioButtonArc2_clicked(bool checked);

    void on_radioButtonCircle2_clicked(bool checked);

    void on_lineEditInner2_textChanged(const QString &arg1);

    void on_lineEditOuter2_textChanged(const QString &arg1);

    void on_lineEditStart2_textChanged(const QString &arg1);

    void on_lineEditEnd2_textChanged(const QString &arg1);

    void on_lineEditNotifTHR2_textChanged(const QString &arg1);

private:
    void initConfig();

    Ui::FrameGZ *ui;

    RadarEngine::RadarConfig* m_instance_cfg;
    GZConfig *gzConfig;
};

#endif // FRAMEGZ_H
