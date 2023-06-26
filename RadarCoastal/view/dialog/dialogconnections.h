#ifndef DIALOGCONNECTIONS_H
#define DIALOGCONNECTIONS_H

#include <QDialog>

#include <RadarEngine/radarconfig.h>

#include "infra/withconfig.h"

namespace Ui {
class DialogConnections;
}

class DialogConnections : public QDialog, protected WithConfig
{
    Q_OBJECT
public:
    explicit DialogConnections(QWidget *parent = nullptr,  RadarEngine::RadarConfig* cfg = nullptr);
    ~DialogConnections() override;
    
signals:
    void signal_settingChange();
    void signal_panTiltPositionEdited(double pan_position,double tilt_position);

private slots:
    void on_checkBoxShow_clicked(bool checked);

    void on_pushButtonApply_clicked();

    void on_pushButtonApplyNav_clicked();

protected:
    void initConfig();

private:
    Ui::DialogConnections *ui;    
};

#endif // DIALOGCONNECTIONS_H
