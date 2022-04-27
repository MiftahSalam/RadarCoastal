#include "dialoggz.h"
#include "ui_dialoggz.h"

DialogGZ::DialogGZ(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGZ)
{
    ui->setupUi(this);
}

DialogGZ::~DialogGZ()
{
    delete ui;
}
