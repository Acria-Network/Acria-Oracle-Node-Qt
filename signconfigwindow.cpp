#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "signconfigwindow.h"
#include "ui_signconfigwindow.h"
#include "signtransaction.h"
#include "util.h"


SignConfigWindow::SignConfigWindow(QWidget *parent, Data* _data) :
    QDialog(parent),
    ui(new Ui::SignConfigWindow)
{
    ui->setupUi(this);

    this->data = _data;

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));
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

void SignConfigWindow::on_pushButton_upload_tooracle_marketplace_clicked()
{
    QJsonObject obj;
    obj["config"] = this->ui->plainTextEdit_sign_config->toPlainText();
    obj["ethereum"] = this->ui->plainTextEdit_signed_ethereum->toPlainText();
    obj["cardano"] = this->ui->plainTextEdit_signed_cardano->toPlainText();
    obj["binance"] = this->ui->plainTextEdit_signed_binance->toPlainText();

    obj["contract_ethereum"] = this->data->chain_data["ethereum"].contract;
    obj["contract_binance"] = this->data->chain_data["binance"].contract;
    obj["contract_cardano"] = this->data->chain_data["cardano"].contract;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    request.setUrl(QUrl("http://127.0.0.1:8000/send_config"));
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, data);
}

void SignConfigWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        Util::show_msgBox(reply->errorString());
        return;
    }

    QString answer = reply->readAll();
    qDebug() << answer;

    Util::show_msgBox(tr("Successfully uploaded signed config!"));
}
