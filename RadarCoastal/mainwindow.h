#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <radarengine.h>

#include "ppi/radarwidget.h"
#include "dialogconnections.h"
#include "dialoggz.h"
#include "dialogbit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    RadarEngine::RadarEngine *m_re;

private slots:
    void trigger_shutdown();

    void on_pushButtonBIT_clicked();

    void on_pushButtonConnections_clicked();

    void on_pushButtonSetGZ_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void setupPPILayout();

    Ui::MainWindow *ui;

    RadarWidget* ppi;
    DialogConnections* dConns;
    DialogGZ *dGZ;
    DialogBIT *dBIT;
};

#endif // MAINWINDOW_H
