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


Resource::Resource()
{
    this->s_value = "";
    this->d_value = 0;
    this->url = "";
    this->contract = "";

    this->state = 0;

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));


    send_manager = new QNetworkAccessManager();

    QObject::connect(send_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(send_managerFinished(QNetworkReply*)));
}

Resource::Resource(QString _url, std::vector<QString> _l_json, QString _contract, QString n, Data* _data, QString _type, uint _id) : Resource()
{
    this->url = _url;
    this->l_json = _l_json;
    this->contract = _contract;
    this->item = n;
    this->data = _data;
    this->type = _type;
    this->id = _id;
}

Resource::~Resource()
{
    delete manager;
    delete send_manager;
}

void Resource::update_resource(){
    request.setUrl(QUrl(url));
    manager->get(request);
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
    QString contract1;
    QString account1;
    unsigned long long transaction_fee = 0;

    if(this->type == "ethereum"){
        url1 = QUrl(this->data->geth_url);
        contract1 = this->data->eth_contract;
        account1 = this->data->eth_account;
        transaction_fee = this->data->transaction_fee_geth;
    }

    else if(this->type == "binance"){
        url1 = QUrl(this->data->binance_url);
        contract1 = this->data->binance_contract;
        account1 = this->data->binance_account;
        transaction_fee = this->data->transaction_fee_binance;
    }

    //QString d1 = "0x01ff927b";
    //d1 = "0xb4883e04";
    QString d1 = "0x6974868e";
    QString d2 = this->item;

    qDebug() << d1;

    for(uint i = d2.size(); i<32;i++){
        d2 += " ";
    }

    QString s = "";
    for(int i = 0; i<d2.size(); i++){
        s += QString::fromStdString(n2hexstr(static_cast<uint>(QString(d2.at(i)).toStdString()[0]), 2));
    }

    qDebug() << s;

    QString d3 = QString::fromStdString(value256.GetHex());


    QString d4 = QString::fromStdString(n2hexstr(this->id));
    for(uint i = d4.size(); i<64;i++){
        d4 = "0" + d4;
    }

    qDebug() << "d4 " << d4 << " " << QString::fromStdString(n2hexstr(3444));

    qDebug() << d3;

    QJsonObject obj1;
    obj1["from"] = account1;
    obj1["to"] = contract1;

    //obj1["gasPrice"] = "0x9184e72a000";
    obj1["gasPrice"] = QString::fromStdString("0x" + n2hexstr(transaction_fee));

    obj1["gas"] = "0x76c00";
    obj1["value"] = "0x0000000000000000000000000000000000000000000000000000000000000000";
    obj1["data"] = d1+s+d3+d4;

    QJsonArray obj3;
    obj3.push_back(obj1);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_sendTransaction";
    obj["params"] = obj3;
    obj["id"] = 22;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    send_request.setUrl(url1);
    send_request.setRawHeader("Content-Type", "application/json");
    send_manager->post(send_request, data);

    this->state=1;
}

void Resource::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
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

void Resource::send_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    qDebug() << answer;

    this->state=2;
}
