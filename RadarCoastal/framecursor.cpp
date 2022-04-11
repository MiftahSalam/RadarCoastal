#include "framecursor.h"
#include "ui_framecursor.h"

FrameCursor::FrameCursor(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameCursor)
{
    ui->setupUi(this);
//    ui->labelCursorLat->hide();
//    ui->labelCursorLon->hide();
}
void FrameCursor::trigger_cursorMove(double range, double brn)
{
    ui->labelCursorRange->setText(QString::number(range,'f',1)+" Km");
    ui->labelCursorBrn->setText(QString::number(brn,'f',1));
}

FrameCursor::~FrameCursor()
{
    delete ui;
}
