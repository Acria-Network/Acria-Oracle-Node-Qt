#ifndef ENTERSINGLEPASSWORDDIALOG_H
#define ENTERSINGLEPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class EnterSinglePasswordDialog;
}

class EnterSinglePasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EnterSinglePasswordDialog(QWidget *parent = nullptr);
    ~EnterSinglePasswordDialog();

    QString password;
    bool password_set;

    void reset();

private slots:
    void on_buttonBox_pass_s_accepted();

private:
    Ui::EnterSinglePasswordDialog *ui;
};

#endif // ENTERSINGLEPASSWORDDIALOG_H
