#ifndef DIALOGBIT_H
#define DIALOGBIT_H

#include "shared/config/navigation_config.h"
#include <QDialog>

#include <RadarEngine/radarconfig.h>

namespace Ui {
class DialogBIT;
}

class DialogBIT : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBIT(QWidget *parent = nullptr);
    ~DialogBIT() override;

private slots:
    void on_pushButtonBITRadar_clicked();

    void on_pushButtonBITNav_clicked();

private:
    Ui::DialogBIT *ui;

    RadarEngine::RadarConfig* m_instance_cfg;
    NavigationConfig *navConfig;

};

#endif // DIALOGBIT_H
