#include "deploywindow.h"
#include "ui_deploywindow.h"
#include "util.h"
#include "json.hpp"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>

DeployWindow::DeployWindow(QWidget *parent, Data *_data, QString _type, ProcessingWindow* _processing_window, QString _hash1) :
    QDialog(parent),
    ui(new Ui::DeployWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    deployed_manager = new QNetworkAccessManager();

    QObject::connect(deployed_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(deployed_managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
    this->state = 3;
    this->hash1 = _hash1;

    this->processing_window = _processing_window;
}

DeployWindow::~DeployWindow()
{
    delete ui;
    delete manager;
}

void DeployWindow::deploy(){
    if(this->state == 3){
        this->state = 0;

        QUrl url1;
        QString contract1;
        QString account1;
        unsigned long long transaction_fee = 0;

        if(this->type == "ethereum"){
            url1 = QUrl(this->data->geth_url);
            contract1 = this->data->eth_contract;
            account1 = this->data->eth_account;
            transaction_fee = this->data->transaction_fee_geth;
        }

        else if(this->type == "binance"){
            url1 = QUrl(this->data->binance_url);
            contract1 = this->data->binance_contract;
            account1 = this->data->binance_account;
            transaction_fee = this->data->transaction_fee_binance;
        }

        QString d1 = "0x1cd23fd9";
        QString d2 = str2bytes32(this->ui->lineEdit_contract_name->text());

        QJsonObject obj1;
        obj1["from"] = account1;
        obj1["to"] = this->ui->lineEdit_main_contract->text();
        obj1["gasPrice"] = QString::fromStdString("0x" + n2hexstr(transaction_fee));
        obj1["gas"] = "0xF4730";
        obj1["value"] = "0x0000000000000000000000000000000000000000000000000000000000000000";
        obj1["data"] = d1+d2;

        QJsonArray obj3;
        obj3.push_back(obj1);

        QJsonObject obj;
        obj["jsonrpc"] = "2.0";
        obj["method"] = "eth_sendTransaction";
        obj["params"] = obj3;
        obj["id"] = 25;
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();

        request.setUrl(url1);
        request.setRawHeader("Content-Type", "application/json");
        manager->post(request, data);

        this->state=1;
    }
}

void DeployWindow::is_deployed(QString hash){
    QUrl url1;
    QString contract1;
    QString account1;
    unsigned long long transaction_fee = 0;

    if(this->type == "ethereum"){
        url1 = QUrl(this->data->geth_url);
        contract1 = this->data->eth_contract;
        account1 = this->data->eth_account;
        transaction_fee = this->data->transaction_fee_geth;
    }

    else if(this->type == "binance"){
        url1 = QUrl(this->data->binance_url);
        contract1 = this->data->binance_contract;
        account1 = this->data->binance_account;
        transaction_fee = this->data->transaction_fee_binance;
    }

    QString d1 = "0x3f83acff";
    QString d2 = str2bytes32(this->ui->lineEdit_contract_name->text());

    QJsonObject obj1;
    obj1["from"] = account1;
    obj1["to"] = this->ui->lineEdit_main_contract->text();
    obj1["data"] = d1+d2;

    QJsonArray obj3;
    obj3.push_back(obj1);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_call";
    obj["params"] = obj3;
    obj["id"] = 79;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    qDebug() << "is_deployed " << QString::fromStdString(data.toStdString());

    deployed_request.setUrl(url1);
    deployed_request.setRawHeader("Content-Type", "application/json");
    deployed_manager->post(deployed_request, data);
}

void DeployWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString();

    qDebug() << answer;

    this->state=2;

    this->hash1 = res;
    this->is_deployed(res);
}

void DeployWindow::deployed_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString();

    qDebug() << res;

    if(res != "0x0000000000000000000000000000000000000000000000000000000000000000"){
        this->processing_window->completed("0x" + res.remove(0, 26));
        this->state=3;
    }
    else{
        this->is_deployed(this->hash1);
    }
}

void DeployWindow::on_buttonBox_accepted()
{
    if(this->state == 3){
        this->processing_window->show();
        this->deploy();
    }
}
