#ifndef DIALOGGZ_H
#define DIALOGGZ_H

#include <QDialog>

#include <RadarEngine/radarconfig.h>

#include "infra/withconfig.h"

namespace Ui {
class DialogGZ;
}

class DialogGZ : public QDialog, protected WithConfig
{
    Q_OBJECT

public:
    explicit DialogGZ(QWidget *parent = nullptr, RadarEngine::RadarConfig* cfg = nullptr);
    ~DialogGZ();

private:
    Ui::DialogGZ *ui;
};

#endif // DIALOGGZ_H
