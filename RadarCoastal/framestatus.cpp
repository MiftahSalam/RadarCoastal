#include "framestatus.h"
#include "ui_framestatus.h"

FrameStatus::FrameStatus(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameStatus)
{
    ui->setupUi(this);
}

FrameStatus::~FrameStatus()
{
    delete ui;
}
