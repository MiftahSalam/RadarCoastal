#ifndef DIALOGCONNECTIONS_H
#define DIALOGCONNECTIONS_H

#include "shared/config/applicationconfig.h"
#include <QDialog>

#include <RadarEngine/radarconfig.h>

namespace Ui {
class DialogConnections;
}

class DialogConnections : public QDialog
{
    Q_OBJECT
public:
    explicit DialogConnections(QWidget *parent = nullptr);
    ~DialogConnections() override;
    
    void upateOpTIme(QString elapsed);

signals:
    void signal_settingChange();
    void signal_panTiltPositionEdited(double pan_position,double tilt_position);

private slots:
    void on_checkBoxShow_clicked(bool checked);

    void on_pushButtonApply_clicked();

    void on_pushButtonApplyNav_clicked();

private:
    void initConfig();

    Ui::DialogConnections *ui;

    RadarEngine::RadarConfig* m_instance_cfg;
    ApplicationConfig *appConfig;
};

#endif // DIALOGCONNECTIONS_H
