#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_re = new RadarEngine::RadarEngine(this);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    qDebug()<<Q_FUNC_INFO<<event->size()<<width()<<height();
    ui->frameControl1->move(0,0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
