#ifndef ENTERPASSWORDDIALOG_H
#define ENTERPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class EnterPasswordDialog;
}

class EnterPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EnterPasswordDialog(QWidget *parent = nullptr);
    ~EnterPasswordDialog();

    QString password;
    bool password_set;

    void reset();

private slots:
    void on_buttonBox_accepted();

    void on_lineEdit_password_1_textChanged(const QString &arg1);

    void on_lineEdit_password_2_textChanged(const QString &arg1);

private:
    Ui::EnterPasswordDialog *ui;
};

#endif // ENTERPASSWORDDIALOG_H
