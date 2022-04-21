#ifndef DIALOGRADAR_H
#define DIALOGRADAR_H

#include <QDialog>

namespace Ui {
class DialogRadar;
}

class DialogRadar : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogRadar(QWidget *parent = 0);
    ~DialogRadar();
    
signals:
    void signal_settingChange();
    void signal_panTiltPositionEdited(double pan_position,double tilt_position);

private slots:
    void on_checkBoxShow_clicked(bool checked);

    void on_pushButtonApply_clicked();

    void on_pushButtonApplyPos_clicked();

private:
    Ui::DialogRadar *ui;
};

#endif // DIALOGRADAR_H
