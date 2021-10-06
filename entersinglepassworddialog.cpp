#include "entersinglepassworddialog.h"
#include "ui_entersinglepassworddialog.h"

#include <QDebug>


EnterSinglePasswordDialog::EnterSinglePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterSinglePasswordDialog)
{
    ui->setupUi(this);
}

EnterSinglePasswordDialog::~EnterSinglePasswordDialog()
{
    delete ui;
}

void EnterSinglePasswordDialog::reset(){
    this->password = "";
    this->password_set = false;
    this->ui->lineEdit_password_1->setText("");
}

void EnterSinglePasswordDialog::on_buttonBox_pass_s_accepted()
{
    qDebug() << 1111111;
    this->password = this->ui->lineEdit_password_1->text();
    this->password_set = true;

    accept();
}
