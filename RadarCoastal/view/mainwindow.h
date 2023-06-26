#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <RadarEngine/radarengine.h>

#include "ppi/radarwidget.h"
#include "view/dialog/dialogconnections.h"
#include "view/dialog/dialoggz.h"
#include "view/dialog/dialogbit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void TriggerShutdown();

    void OnPushButtonBITClicked();

    void OnPushButtonConnectionsClicked();

    void OnPushButtonSetGZClicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void setupPPILayout();

    Ui::MainWindow *ui;

    RadarEngine::RadarEngine* m_re;
    RadarEngine::RadarConfig* m_cfg;
    RadarWidget* m_ppi;
    DialogConnections* m_dialog_conns;
    DialogGZ *m_dialog_gz;
    DialogBIT *m_dialog_bit;
};

#endif // MAINWINDOW_H
