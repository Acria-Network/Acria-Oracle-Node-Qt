#include "compinfo.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>

#include "util.h"
#include "json.hpp"


compinfo::compinfo(Data *_data)
{
    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    filter_manager = new QNetworkAccessManager();

    QObject::connect(filter_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(filter_managerFinished(QNetworkReply*)));

    this->data = _data;
    this->filter = "";

    create_filter_events();
}

compinfo::~compinfo(){
    delete manager;
    delete filter_manager;
}

void compinfo::create_filter_events(){

    QJsonArray addr;
    addr.push_back(this->data->eth_contract);

    QJsonArray top;
    top.push_back("0x80159d42b8a6ee3e969f3e2c24f97a6835bebbb529a24ca1fbb217e4f7701240");

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

    filter_request.setUrl(QUrl(this->data->geth_url));
    filter_request.setRawHeader("Content-Type", "application/json");
    filter_manager->post(filter_request, data);
}

void compinfo::update_events(){
    if(this->filter != ""){
        QJsonArray obj3;
        obj3.push_back(this->filter);

        QJsonObject obj;
        obj["jsonrpc"] = "2.0";
        obj["method"] = "eth_getFilterLogs";
        obj["params"] = obj3;
        obj["id"] = 66;
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();

        request.setUrl(QUrl(this->data->geth_url));
        request.setRawHeader("Content-Type", "application/json");
        manager->post(request, data);
    }
}

void compinfo::filter_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString();

    qDebug() << answer;
    qDebug() << res;

    this->filter = res;
}

void compinfo::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    comp c;

    QString answer = reply->readAll();

    qDebug() << answer;

    nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());

    this->completed.clear();

    for(uint i = 0;i<tmp1["result"].size();i++){
        c.hash = QString::fromStdString(tmp1["result"][i]["transactionHash"]);


        c.address = QString::fromStdString(tmp1["result"][i]["address"]);
        c.block = QString::fromStdString(tmp1["result"][i]["blockNumber"]).remove(0,2).toUInt(NULL, 16);

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

        c.requestID = hex2str(inf[0]).trimmed();
        c.callback = inf[1].remove(0,24);

        qDebug() << c.requestID;
        qDebug() << c.callback;

        this->completed.push_back(c);
    }
}
