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

    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);
}

NonceManager::~NonceManager(){
    delete manager;
}

void NonceManager::reset(){
    this->ready = false;
    this->nonce = 0;

    this->update_nonce();
}

unsigned NonceManager::get_nonce(){
    QMutexLocker ml(&mutex);

    qDebug() << this->type << " nonce incremented from: " << this->nonce << " to: " << this->nonce+1;
    this->nonce+=1;
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
        qDebug() << "Error (nonce manager): " << this->type << ": " << reply->errorString();
        this->ready = false;
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = Util::ObjectFromString(answer);

    qDebug() << "New " << this->type << " nonce: " << obj["result"].toString().toUInt(NULL, 16);

    if(obj["result"].toString() != "" && obj["result"].toString() != nullptr)
        this->ready = true;
    else
        this->ready = false;

    this->nonce = obj["result"].toString().toUInt(NULL, 16) - 1;
    this->ready = true;
}
