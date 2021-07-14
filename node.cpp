#include "node.h"
#include "util.h"

#include <QString>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>


Node::Node(Data* _data, QString _type)
{
    this->data = _data;
    this->type = _type;

    status_manager = new QNetworkAccessManager();

    QObject::connect(status_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(statusManagerFinished(QNetworkReply*)));

    chain_id_manager = new QNetworkAccessManager();

    QObject::connect(chain_id_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(chain_idManagerFinished(QNetworkReply*)));

    balance_manager = new QNetworkAccessManager();

    QObject::connect(balance_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(balanceManagerFinished(QNetworkReply*)));

    this->status_geth = false;

    QSslConfiguration conf = status_request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    status_request.setSslConfiguration(conf);
    chain_id_request.setSslConfiguration(conf);
    balance_request.setSslConfiguration(conf);
};

void Node::update_user_balance(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    QJsonArray obj99;
    obj99.push_back(account1);
    obj99.push_back("latest");

    QJsonObject obj3;
    obj3["jsonrpc"] = "2.0";
    obj3["method"] = "eth_getBalance";
    obj3["params"] = obj99;
    obj3["id"] = 63;
    QJsonDocument doc3(obj3);
    QByteArray data3 = doc3.toJson();

    balance_request.setUrl(url1);
    balance_request.setRawHeader("Content-Type", "application/json");
    balance_manager->post(balance_request, data3);
}

void Node::update_geth_status(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "web3_clientVersion";
    obj["id"] = 67;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    status_request.setUrl(url1);
    status_request.setRawHeader("Content-Type", "application/json");
    status_manager->post(status_request, data);

    QJsonObject obj2;
    obj2["jsonrpc"] = "2.0";
    obj2["method"] = "eth_chainId";
    obj2["id"] = 69;
    QJsonDocument doc2(obj2);
    QByteArray data2 = doc2.toJson();

    chain_id_request.setUrl(url1);
    chain_id_request.setRawHeader("Content-Type", "application/json");
    chain_id_manager->post(chain_id_request, data2);

    QJsonArray obj99;
    obj99.push_back(account1);
    obj99.push_back("latest");

    QJsonObject obj3;
    obj3["jsonrpc"] = "2.0";
    obj3["method"] = "eth_getBalance";
    obj3["params"] = obj99;
    obj3["id"] = 63;
    QJsonDocument doc3(obj3);
    QByteArray data3 = doc3.toJson();

    balance_request.setUrl(url1);
    balance_request.setRawHeader("Content-Type", "application/json");
    balance_manager->post(balance_request, data3);
}

Node::~Node()
{
    delete status_manager;
};

void Node::statusManagerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error (geth status): " << this->type << ": " << reply->errorString();
        status_geth = false;
        return;
    }

    QString answer = reply->readAll();

    qDebug() << "Geth status: " << this->type << ": " << answer;

    QJsonObject obj = Util::ObjectFromString(answer);

    this->geth_version = obj["result"].toString();

    if(this->geth_version != "" && this->geth_version != nullptr)
        status_geth = true;
    else
        status_geth = false;
}

void Node::chain_idManagerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error (chain id): " << this->type << ": " << reply->errorString();
        status_geth_chain_id = false;
        return;
    }

    QString answer = reply->readAll();

    qDebug() << "Chain Id: " << this->type << ": " << answer;

    QJsonObject obj = Util::ObjectFromString(answer);

    if(obj["result"].toString() != "" && obj["result"].toString() != nullptr)
        status_geth_chain_id = true;
    else
        status_geth_chain_id = false;

    this->chain_id = obj["result"].toString().toUInt(NULL, 16);
    this->data->chain_data[this->type].chain_id = this->chain_id;
}

void Node::balanceManagerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error (balance account): " << this->type << ": " << reply->errorString();
        this->data->chain_data[this->type].balance = 0;
        return;
    }

    QString answer = reply->readAll();

    qDebug() << "Balance account: " << this->type << ": " << answer;

    QJsonObject obj = Util::ObjectFromString(answer);

    if(obj["result"].toString() != "" && obj["result"].toString() != nullptr)
        this->data->chain_data[this->type].balance = Util::toUint128(obj["result"].toString());
    else
        this->data->chain_data[this->type].balance = 0;
}

bool Node::get_status_geth(){
    if(status_geth_chain_id && status_geth)
        return true;
    else
        return false;
}
