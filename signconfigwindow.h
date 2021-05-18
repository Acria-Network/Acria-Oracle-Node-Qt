#ifndef SIGNCONFIGWINDOW_H
#define SIGNCONFIGWINDOW_H

#include <QDialog>
#include "data.h"

namespace Ui {
class SignConfigWindow;
}

class SignConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SignConfigWindow(QWidget *parent = nullptr, Data* _data = nullptr);
    ~SignConfigWindow();

    void sign_all(QString config);

private:
    Ui::SignConfigWindow *ui;

    Data* data;
};

#endif // SIGNCONFIGWINDOW_H
