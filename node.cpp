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

    this->status_geth = false;

    update_geth_status();
};

void Node::update_geth_status(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    qDebug() << "answer";
    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "web3_clientVersion";
    obj["params"] = "";
    obj["id"] = 67;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    status_request.setUrl(url1);
    status_request.setRawHeader("Content-Type", "application/json");
    status_manager->post(status_request, data);
}

Node::~Node()
{
    delete status_manager;
};

void Node::statusManagerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "error " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    qDebug() << "answer " <<answer;

    QJsonObject obj = Util::ObjectFromString(answer);

    qDebug() << obj["result"].toString();

    this->geth_version = obj["result"].toString();

    if(this->geth_version != "" && this->geth_version != nullptr)
        status_geth = true;
    else
        status_geth = false;
}

bool Node::get_status_geth(){
    return status_geth;
}
