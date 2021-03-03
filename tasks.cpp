#include "tasks.h"
#include "util.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>

Tasks::Tasks(Data *_data, QString _type)
{
    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    r_manager = new QNetworkAccessManager();

    QObject::connect(r_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(r_managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
}

Tasks::~Tasks()
{
    delete manager;
    delete r_manager;
}

void Tasks::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString().remove(0, 2);

    qDebug() << res;

    if(res.length() > 2){
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

        for(uint i = 0; i<inf.size(); i++){
            qDebug() << inf[i];
        }

        bool bStatus = false;
        uint nHex = inf[1].toUInt(&bStatus,16);

        if(this->type == "ethereum")
            this->data->items.clear();
        else if(this->type == "binance")
            this->data->binance_items.clear();

        for(uint i=2;i<2+nHex;i++){
            QString tmp = "";
            for(int f = 1; f< inf[i].length(); f+=2){
                tmp += QString(static_cast<char>((QString(inf[i].at(f-1)) + QString(inf[i].at(f))).toUInt(NULL,16)));
            }
            qDebug() << tmp.trimmed();

            if(this->type == "ethereum")
                this->data->items.push_back(tmp.trimmed());
            else if(this->type == "binance")
                this->data->binance_items.push_back(tmp.trimmed());
        }
    }

}

void Tasks::r_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString().remove(0, 2);

    qDebug() << res;

    if(res.length() > 2){
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

        for(uint i = 0; i<inf.size(); i++){
            qDebug() << inf[i];
        }

        bool bStatus = false;
        uint nHex = inf[1].toUInt(&bStatus,16);

        if(nHex != 0){
            uint arr_size = (inf.size()-2)/nHex;

            this->requests.clear();

            for(uint i=2;i<inf.size();i+=arr_size){
                if(!(inf[i].toUInt(NULL,16) == 0 && inf[i+1].toUInt(NULL,16) == 0 && inf[i+2].toUInt(NULL,16) == 0 && inf[i+3].toUInt(NULL,16) == 0 && inf[i+4].toUInt(NULL,16) == 0)){
                    req r;

                    QString tmp = "";
                    for(int f = 1; f< inf[i].length(); f+=2){
                        tmp += QString(static_cast<char>((QString(inf[i].at(f-1)) + QString(inf[i].at(f))).toUInt(NULL,16)));
                    }
                    qDebug() << "r.requestID " << tmp.trimmed();

                    r.requestID = tmp.trimmed();


                    r.cancelled = static_cast<bool>(QString(inf[i+1].at(inf[i+1].length()-1)).toUInt(NULL,16));
                    qDebug() << r.cancelled;

                    r.expiration = QString(inf[i+2]).toUInt(NULL,16);
                    qDebug() << r.expiration;

                    r.callback = inf[i+3].remove(0, 24);
                    qDebug() << r.callback;

                    r.chain = this->type;

                    r.id = QString(inf[i+4]).toUInt(NULL,16);

                    this->requests.push_back(r);
                }
            }
        }
        else{
            this->requests.clear();
        }
    }
}

void Tasks::update_tasks(){
    QUrl url1;
    QString contract;
    QString account;

    if(this->type == "ethereum"){
        url1 = QUrl(this->data->geth_url);
        contract = this->data->eth_contract;
        account = this->data->eth_account;
    }

    else if(this->type == "binance"){
        url1 = QUrl(this->data->binance_url);
        contract = this->data->binance_contract;
        account = this->data->binance_account;
    }

    QJsonObject obj1;
    obj1["from"] = account;
    obj1["to"] = contract;
    obj1["data"] = "0xb715c7060000000000000000000000000000000000000000000000000000000000000000";

    QJsonArray obj3;
    obj3.push_back(obj1);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_call";
    obj["params"] = obj3;
    obj["id"] = 77;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, data);
}

void Tasks::update_requests(){
    QUrl url1;
    QString contract;
    QString account;

    if(this->type == "ethereum"){
        url1 = QUrl(this->data->geth_url);
        contract = this->data->eth_contract;
        account = this->data->eth_account;
    }

    else if(this->type == "binance"){
        url1 = QUrl(this->data->binance_url);
        contract = this->data->binance_contract;
        account = this->data->binance_account;
    }

    QJsonObject obj1;
    obj1["from"] = account;
    obj1["to"] = contract;
    obj1["data"] = "0x455bfbf20000000000000000000000000000000000000000000000000000000000000000";

    QJsonArray obj3;
    obj3.push_back(obj1);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_call";
    obj["params"] = obj3;
    obj["id"] = 77;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    r_request.setUrl(url1);
    r_request.setRawHeader("Content-Type", "application/json");
    r_manager->post(r_request, data);
}
