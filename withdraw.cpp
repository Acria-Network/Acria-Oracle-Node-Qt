#include "withdraw.h"
#include "processingwindow.h"
#include "util.h"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>

Withdraw::Withdraw(Data *_data, QString _type)
{
    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
}

Withdraw::~Withdraw(){
    delete manager;
}

void Withdraw::withdraw(ProcessingWindow* _processing_window){
    this->processing_window = _processing_window;

    QUrl url1;
    QString contract1;
    QString account1;

    if(this->type == "ethereum"){
        url1 = QUrl(this->data->geth_url);
        contract1 = this->data->eth_contract;
        account1 = this->data->eth_account;
    }

    else if(this->type == "binance"){
        url1 = QUrl(this->data->binance_url);
        contract1 = this->data->binance_contract;
        account1 = this->data->binance_account;
    }

    QString d1 = "0x3ccfd60b";

    qDebug() << d1;

    QJsonObject obj1;
    obj1["from"] = account1;
    obj1["to"] = contract1;
    obj1["gasPrice"] = "0x9184e72a000";
    obj1["gas"] = "0x76c00";
    obj1["value"] = "0x0000000000000000000000000000000000000000000000000000000000000000";
    obj1["data"] = d1;

    QJsonArray obj3;
    obj3.push_back(obj1);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_sendTransaction";
    obj["params"] = obj3;
    obj["id"] = 11;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, data);
}

void Withdraw::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString();

    qDebug() << answer;

    this->processing_window->completed(res);
}
