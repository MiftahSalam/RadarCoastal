#include "dialoggz.h"
#include "ui_dialoggz.h"

DialogGZ::DialogGZ(QWidget *parent, RadarEngine::RadarConfig *cfg) :
    QDialog(parent),WithConfig(cfg),
    ui(new Ui::DialogGZ)
{
    ui->setupUi(this);
}

DialogGZ::~DialogGZ()
{
    delete ui;
}
