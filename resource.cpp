#include "resource.h"
#include "util.h"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonArray>
#include <math.h>
#include <iomanip>
#include <string>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <keccak256.h>
#include <QThread>
#include <QTimer>


Resource::Resource()
{
    this->s_value = "";
    this->d_value = 0;
    this->url = "";
    this->contract = "";

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));


    send_manager = new QNetworkAccessManager();

    QObject::connect(send_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(send_managerFinished(QNetworkReply*)));


    deployed_manager = new QNetworkAccessManager();

    QObject::connect(deployed_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(deployed_managerFinished(QNetworkReply*)));

    is_deployed_timer = new QTimer(this);
    connect(is_deployed_timer, SIGNAL(timeout()), this, SLOT(is_deployed()));
}

Resource::Resource(QString _url, std::vector<QString> _l_json, QString _contract, QString n, Data* _data, QString _type, uint _id, uint* state, uint _max_gas, unsigned long long _fee, NonceManager* _nonce_manager) : Resource()
{
    this->url = _url;
    this->l_json = _l_json;
    this->contract = _contract;
    this->item = n;
    this->data = _data;
    this->type = _type;
    this->id = _id;
    this->state = state;
    this->max_gas = _max_gas;
    this->fee = _fee;
    this->nonce_manager = _nonce_manager;

    this->nonce = this->nonce_manager->get_nonce();
}

Resource::~Resource()
{
    delete manager;
    delete send_manager;
    delete is_deployed_timer;
}

void Resource::update_resource(){
    if(this->get_minimum_transaction_fee() < this->fee){
        request.setUrl(QUrl(url));
        manager->get(request);
    }
    else{
        QThread::msleep(30000);
        this->update_resource();
    }
}

std::string digits = "0123456789abcdef";
std::string tohex(std::string number) {                        // Decimal to Hexadecimal function
    long length = number.length();
    std::string result = "";
    std::vector<long> nibbles;
    for ( long i = 0; i < length; i++ ) {
        nibbles.push_back(digits.find(number[i]));
    }
    long newlen = 0;
    do {
        long value = 0;
        newlen = 0;
        for ( long i = 0; i < length; i++ ) {
            value = (value * 10) + nibbles[i];
            if (value >= 16) {
                nibbles[newlen++] = value / 16;
                value %= 16;
            } else if (newlen > 0) {
                nibbles[newlen++] = 0;
            };
        };
        length = newlen;
        result = digits[value] + result;
    } while (newlen != 0);
    return result;
}

void Resource::send_resource(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    Keccak keccak;

    QString d1 = "0x" + QString::fromStdString(keccak("fillRequest(bytes8,uint256,uint256)")).mid(0,8);
    qDebug()<< "d1 " <<d1;
    QString d2 = Util::str2bytes8(this->item)+"000000000000000000000000000000000000000000000000";

    qDebug() << d1;
    qDebug() << d2;

    QString d3 = QString::fromStdString(value256.GetHex());

    QString d4 = QString::fromStdString(Util::n2hexstr(this->id));
    for(uint i = d4.size(); i<64;i++){
        d4 = "0" + d4;
    }

    qDebug() << d4;
    qDebug() << d3;

    QString data1 = d1+d2+d3+d4;

    send_request.setUrl(url1);
    send_request.setRawHeader("Content-Type", "application/json");
    send_manager->post(send_request, Util::generate_rpc_call("eth_sendTransaction", account1, contract1, data1, transaction_fee, 486400, 72, this->nonce));

    *this->state=1;
}

void Resource::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        this->error = reply->errorString();
        *this->state=8;
        return;
    }

    QString answer = reply->readAll();
    nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());

    double ans = static_cast<double>(tmp1[l_json[0].toStdString()][l_json[1].toStdString()]);

    d_value=ans;

    QString tmp2 = QString::number(d_value);
    int point = tmp2.indexOf('.');

    qDebug() << "point" << point;
    tmp2 = tmp2.replace(".", "");

    for(uint i=tmp2.length()-point;i<18;i++){
        tmp2 += "0";
    }

    value256 = uint256S(tohex(tmp2.toStdString()));

    qDebug() << QString::fromStdString(value256.ToString());


    send_resource();
}

unsigned long long Resource::get_minimum_transaction_fee(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    unsigned long long total_gas = max_gas + 40000;

    unsigned long long mfee = total_gas*transaction_fee;
    qDebug() << "minimum fee " << mfee;
    qDebug() << "paid fee " << fee;

    return mfee;
}

void Resource::send_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        this->error = reply->errorString();
        *this->state=9;
        return;
    }

    QString answer = reply->readAll();

    qDebug() << answer;

    *this->state=2;

    QJsonObject obj = Util::ObjectFromString(answer);
    QString res = obj["result"].toString();

    qDebug() << answer;

    this->hash = res;
    //this->is_deployed();
    is_deployed_timer->start(10000);
}

void Resource::is_deployed(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    QJsonArray obj3;
    obj3.push_back(hash);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_getTransactionByHash";
    obj["params"] = obj3;
    obj["id"] = 26;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    deployed_request.setUrl(url1);
    deployed_request.setRawHeader("Content-Type", "application/json");
    deployed_manager->post(deployed_request, data);
}

void Resource::deployed_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        this->error = reply->errorString();
        *this->state=10;
        return;
    }

    QString answer = reply->readAll();
    nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());

    qDebug() << "deployed " << answer;

    if(tmp1["result"]["blockNumber"] != nullptr){
        *this->state=3;
        qDebug() << "completed transaction: " << QString::fromStdString(tmp1["result"]["blockNumber"]);
    }
    else{
        is_deployed_timer->start(10000);
    }
}
