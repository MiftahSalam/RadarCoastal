#ifndef DIALOGGZ_H
#define DIALOGGZ_H

#include <QDialog>

namespace Ui {
class DialogGZ;
}

class DialogGZ : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGZ(QWidget *parent = nullptr);
    ~DialogGZ();

private:
    Ui::DialogGZ *ui;
};

#endif // DIALOGGZ_H
