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
    QJsonObject obj = Util::ObjectFromString(answer);
    QString res = obj["result"].toString().remove(0, 2);

    qDebug() << answer;

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

        uint nHex = inf[1].toUInt(NULL,16);

        this->data->items_clear(this->type);

        for(uint i=2;i<2+nHex;i++){
            QString tmp = "";

            for(int f = 1; f< inf[i].length(); f+=2){
                tmp += QString(static_cast<char>((QString(inf[i].at(f-1)) + QString(inf[i].at(f))).toUInt(NULL,16)));
            }

            this->data->item_push_back(this->type, tmp.trimmed());
        }
    }

}

void Tasks::r_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "error tasks: " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = Util::ObjectFromString(answer);
    QString res = obj["result"].toString().remove(0, 2);

    qDebug() << answer;

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

        uint nHex = inf[1].toUInt(NULL,16);

        if(nHex != 0){
            uint arr_size = (inf.size()-2)/nHex;

            this->requests.clear();

            for(uint i=2;i<inf.size();i+=arr_size){
                if(!(inf[i].toUInt(NULL,16) == 0 && inf[i+1].toUInt(NULL,16) == 0 && inf[i+2].toUInt(NULL,16) == 0 && inf[i+3].toUInt(NULL,16) == 0 && inf[i+4].toUInt(NULL,16) == 0)){
                    req r;

                    QString tmp = "";
                    for(int f = 1; f< inf[i].length(); f+=2){
                        uint char0 = (QString(inf[i].at(f-1)) + QString(inf[i].at(f))).toUInt(NULL,16);
                        if(char0 != 0)
                            tmp += QString(static_cast<char>(char0));
                    }

                    r.requestID = tmp.trimmed();
                    r.fee = QString(inf[i+1]).toULongLong(NULL,16);
                    r.expiration = QString(inf[i+2]).toUInt(NULL,16);
                    r.callback = inf[i+5].remove(0, 24);
                    r.chain = this->type;
                    r.id = QString(inf[i+3]).toUInt(NULL,16);
                    r.max_gas = QString(inf[i+4]).toUInt(NULL, 16);
                    if(QString(inf[i+6]).replace("0", "") != ""){
                        qDebug() << "request data: " <<  QString(inf[i+6]);
                        r.data = QString(inf[i+6]);
                    }

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
    QString contract, account;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account, &contract, &transaction_fee);

    QString data1 = "0xb715c7060000000000000000000000000000000000000000000000000000000000000000";

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, Util::generate_rpc_call("eth_call", account, contract, data1, transaction_fee, 0, 77, -1));
}

void Tasks::update_requests(){
    QUrl url1;
    QString contract, account;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account, &contract, &transaction_fee);

    QString data1 = "0x455bfbf20000000000000000000000000000000000000000000000000000000000000000";

    r_request.setUrl(url1);
    r_request.setRawHeader("Content-Type", "application/json");
    r_manager->post(r_request, Util::generate_rpc_call("eth_call", account, contract, data1, 0, 0, 78, -1));
}
