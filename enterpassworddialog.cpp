#include "enterpassworddialog.h"
#include "ui_enterpassworddialog.h"

EnterPasswordDialog::EnterPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterPasswordDialog)
{
    ui->setupUi(this);

    this->ui->buttonBox->setDisabled(true);
    this->password_set = false;
}

EnterPasswordDialog::~EnterPasswordDialog()
{
    delete ui;
}

void EnterPasswordDialog::reset()
{
    this->ui->buttonBox->setDisabled(true);
    this->password = "";
    this->password_set = false;
}

void EnterPasswordDialog::on_buttonBox_accepted()
{
    this->password = this->ui->lineEdit_password_1->text();
    this->password_set = true;
}

void EnterPasswordDialog::on_lineEdit_password_1_textChanged(const QString &arg1)
{
    if(arg1 == this->ui->lineEdit_password_2->text() && arg1 != ""){
        this->ui->buttonBox->setDisabled(false);
    }
    else{
        this->ui->buttonBox->setDisabled(true);
    }
}

void EnterPasswordDialog::on_lineEdit_password_2_textChanged(const QString &arg1)
{
    if(arg1 == this->ui->lineEdit_password_1->text() && arg1 != ""){
        this->ui->buttonBox->setDisabled(false);
    }
    else{
        this->ui->buttonBox->setDisabled(true);
    }
}
