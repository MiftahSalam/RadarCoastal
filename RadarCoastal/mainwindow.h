#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <radarengine.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    RadarEngine::RadarEngine *m_re;

private slots:
    void trigger_shutdown();

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
