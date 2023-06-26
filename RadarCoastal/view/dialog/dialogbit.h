#ifndef DIALOGBIT_H
#define DIALOGBIT_H

#include <QDialog>

#include <RadarEngine/radarconfig.h>

#include "infra/withconfig.h"

namespace Ui {
class DialogBIT;
}

class DialogBIT : public QDialog, protected WithConfig
{
    Q_OBJECT

public:
    explicit DialogBIT(QWidget *parent = nullptr, RadarEngine::RadarConfig* cfg = nullptr);
    ~DialogBIT() override;

private slots:
    void on_pushButtonBITRadar_clicked();

    void on_pushButtonBITLora_clicked();

    void on_pushButtonBITNav_clicked();

    void on_pushButtonBITCamera_clicked();

private:
    Ui::DialogBIT *ui;
};

#endif // DIALOGBIT_H
