#include "signconfigwindow.h"
#include "ui_signconfigwindow.h"
#include "signtransaction.h"


SignConfigWindow::SignConfigWindow(QWidget *parent, Data* _data) :
    QDialog(parent),
    ui(new Ui::SignConfigWindow)
{
    ui->setupUi(this);

    this->data = _data;
}

SignConfigWindow::~SignConfigWindow()
{
    delete ui;
}

void SignConfigWindow::sign_all(QString config){
    this->ui->plainTextEdit_sign_config->setPlainText(config);
    if(data->chain_data["ethereum"].private_key != "")
        this->ui->plainTextEdit_signed_ethereum->setPlainText(QString::fromStdString(SignTransaction::sign_message(config, data->chain_data["ethereum"].private_key.toStdString())));
    if(data->chain_data["binance"].private_key != "")
        this->ui->plainTextEdit_signed_binance->setPlainText(QString::fromStdString(SignTransaction::sign_message(config, data->chain_data["binance"].private_key.toStdString())));
    if(data->chain_data["cardano"].private_key != "")
        this->ui->plainTextEdit_signed_cardano->setPlainText(QString::fromStdString(SignTransaction::sign_message(config, data->chain_data["cardano"].private_key.toStdString())));
}
