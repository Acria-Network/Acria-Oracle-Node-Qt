#include "balances.h"
#include "util.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>

Balances::Balances(Data *_data, QString _type)
{
    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;

    this->withdrawable = 0;

    update_withdrawable();
}

Balances::~Balances(){
    delete manager;
}

void Balances::update_withdrawable(){
    QUrl url1;
    QString contract, account;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account, &contract, &transaction_fee);

    QString data1 = "0x64245d010000000000000000000000000000000000000000000000000000000000000000";

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, generate_rpc_call("eth_call", account, contract, data1, transaction_fee, 0, 77));
}

void Balances::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString().remove(0, 2);

    qDebug() << res;

    if(res.length() > 2){
        qDebug() << "withdrawable " << res;

        this->withdrawable = res.toULongLong(NULL,16);
    }

}
