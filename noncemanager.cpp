#include "noncemanager.h"
#include "util.h"

#include <QNetworkReply>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>


NonceManager::NonceManager(Data* _data, QString _type)
{
    this->ready = false;
    this->nonce = 0;
    this->type = _type;
    this->data = _data;

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->update_nonce();
}

unsigned NonceManager::get_nonce(){
    return this->nonce;
}

void NonceManager::update_nonce(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    QJsonArray obj3;
    obj3.push_back(account1);
    obj3.push_back("latest");

    qDebug() << "answer";
    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_getTransactionCount";
    obj["params"] = obj3;
    obj["id"] = 27;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, data);
}

void NonceManager::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "error " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    qDebug() << "answer " <<answer;

    QJsonObject obj = ObjectFromString(answer);

    qDebug() << obj["result"].toString();

    qDebug() << "nonce: " << obj["result"].toString().toUInt(NULL, 16);

    this->nonce = obj["result"].toString().toUInt(NULL, 16);
    this->ready = true;
}
