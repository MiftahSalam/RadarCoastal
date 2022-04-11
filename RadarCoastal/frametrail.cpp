#include "frametrail.h"
#include "ui_frametrail.h"

FrameTrail::FrameTrail(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameTrail)
{
    ui->setupUi(this);
}

FrameTrail::~FrameTrail()
{
    delete ui;
}
