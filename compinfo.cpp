#include "compinfo.h"
#include "util.h"
#include "json.hpp"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>


compinfo::compinfo(Data *_data, QString _type)
{
    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    filter_manager = new QNetworkAccessManager();

    QObject::connect(filter_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(filter_managerFinished(QNetworkReply*)));

    this->data = _data;
    this->filter = "";
    this->type = _type;

    create_filter_events();
}

compinfo::~compinfo(){
    delete manager;
    delete filter_manager;
}

void compinfo::create_filter_events(){
    QUrl url1;
    QString contract, account;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account, &contract, &transaction_fee);

    QJsonArray addr;
    addr.push_back(contract);

    QJsonArray top;
    top.push_back("0x910c973544a08bc7a636ea9436726896a35fb16763f6a536991139fa41a7f0e5");

    QJsonObject obj1;
    obj1["fromBlock"] = "0x1";
    obj1["toBlock"] = "latest";
    obj1["address"] = addr;
    obj1["topics"] = top;

    QJsonArray obj3;
    obj3.push_back(obj1);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_newFilter";
    obj["params"] = obj3;
    obj["id"] = 66;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    filter_request.setUrl(url1);
    filter_request.setRawHeader("Content-Type", "application/json");
    filter_manager->post(filter_request, data);
}

void compinfo::update_events(){
    if(this->filter != ""){
        QUrl url1;
        QString contract, account;
        unsigned long long transaction_fee = 0;

        this->data->get_chain_info(this->type, &url1, &account, &contract, &transaction_fee);

        QJsonArray obj3;
        obj3.push_back(this->filter);

        QJsonObject obj;
        obj["jsonrpc"] = "2.0";
        obj["method"] = "eth_getFilterLogs";
        obj["params"] = obj3;
        obj["id"] = 66;
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();

        request.setUrl(url1);
        request.setRawHeader("Content-Type", "application/json");
        manager->post(request, data);
    }
}

void compinfo::filter_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error (filter completed requests): " << this->type << ": " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = Util::ObjectFromString(answer);
    QString res = obj["result"].toString();

    this->filter = res;
}

void compinfo::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error (completed requests): " << this->type << ": " << reply->errorString();
        return;
    }

    comp c;

    QString answer = reply->readAll();

    nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());

    this->completed.clear();

    for(uint i = 0;i<tmp1["result"].size();i++){
        c.hash = QString::fromStdString(tmp1["result"][i]["transactionHash"]);


        c.address = QString::fromStdString(tmp1["result"][i]["address"]);
        c.block = QString::fromStdString(tmp1["result"][i]["blockNumber"]).remove(0,2).toUInt(NULL, 16);

        c.chain = this->type;

        QString res = QString::fromStdString(tmp1["result"][i]["data"]).remove(0,2);
        std::vector<QString> inf;
        QString res1 = "";
        uint u = 0;

        for(int i = 0; i< res.length(); i++)
        {
            if(u >= 64){
                inf.push_back(res1);
                res1 = "";
                u = 0;
            }

            res1 += res.at(i);
            u++;
        }
        inf.push_back(res1);

        c.requestID = Util::hex2str(inf[0]).trimmed();
        c.callback = inf[1].remove(0,24);
        c.id = QString(inf[3]).toUInt(NULL,16);
        c.fee = QString(inf[2]).toULongLong(NULL,16);

        this->completed.push_back(c);
    }
}
