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
#include "signtransaction.h"


Resource::Resource()
{
    this->s_value = "";
    this->d_value = 0;
    this->url = "";
    this->regex = "";

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

Resource::Resource(QString _url, std::vector<QString> _l_json, QString _regex, QString n, Data* _data, QString _type, uint _id, uint* state, uint _max_gas, unsigned long long _fee, QString _request_data, QString _url_data, QString _parameter_type, NonceManager* _nonce_manager) : Resource()
{
    this->url = _url;
    this->l_json = _l_json;
    this->item = n;
    this->data = _data;
    this->type = _type;
    this->id = _id;
    this->state = state;
    this->max_gas = _max_gas;
    this->fee = _fee;
    this->nonce_manager = _nonce_manager;
    this->request_data = _request_data;
    this->url_data = _url_data;
    this->parameter_type = _parameter_type;
    this->regex = _regex;

    for(int i = l_json.size()-1; i>=0; i--){
        if(l_json[i] == ""){
            l_json.erase(l_json.begin() + i);
        }
        else
            break;
    }

    //this->nonce = this->nonce_manager->get_nonce();
}

Resource::~Resource()
{
    delete manager;
    delete send_manager;
    delete is_deployed_timer;
}

QString Resource::convert_parameter(QString _parameter_type, QString _request_data){
    QStringList data = _parameter_type.split(" => ");
    QString str = _request_data;

    for(int i = 0; i<data.size(); i++){
        if(data.at(i) == "timestamp" || data.at(i) == "number"){
            str = QString::number(str.toULongLong(NULL, 16));
        }
        else if(data.at(i).indexOf("from_timestamp ") != -1){
            QString format = QString(data.at(i)).replace("from_timestamp ", "");
            QDateTime timestamp;
            timestamp.setTime_t(str.toULongLong());
            str = timestamp.toString(format);
        }
        else if(data.at(i) == "string"){
            str = Util::hex2str(str).trimmed();
        }
        else if(data.at(i) == "address"){
            str = str.remove(0, 24);
        }
    }

    return str;
}

void Resource::update_resource(){
    if(this->get_minimum_transaction_fee() < this->fee){
        qDebug() << "request_data: " << this->request_data;
        if(this->request_data != "")
            request.setUrl(QUrl(url_data.replace("%data%", this->convert_parameter(this->parameter_type, this->request_data))));
        else
            request.setUrl(QUrl(url));

        manager->get(request);
    }
    else{
        QThread::msleep(30000);
        this->update_resource();
    }
}

void Resource::send_resource(){
    this->nonce = this->nonce_manager->get_nonce();

    QUrl url1;
    QString contract1, account1, privkey;
    unsigned long long transaction_fee = 0; unsigned chain_id = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee, &privkey, &chain_id);

    privkey.remove(0, 2);

    Keccak keccak;

    QString d1 = "0x" + QString::fromStdString(keccak("fillRequest(bytes8,uint256,uint256)")).mid(0,8);
    QString d2 = Util::str2bytes8(this->item)+"000000000000000000000000000000000000000000000000";
    QString d3 = QString::fromStdString(value256.GetHex());
    QString d4 = QString::fromStdString(Util::n2hexstr(this->id));
    for(uint i = d4.size(); i<64;i++){
        d4 = "0" + d4;
    }

    QString data1 = d1+d2+d3+d4;

    Transaction tx;
    std::string nonce_ = RLP::intToHex(nonce);
    if(nonce == 0)
        nonce_ = "";
    tx.nonce=SignTransaction::fixHexValue(nonce_);
    //tx.gasPrice=SignTransaction::fixHexValue("0x4A817C800");
    tx.gasPrice=SignTransaction::fixHexValue(RLP::intToHex(transaction_fee));
    tx.gasLimit=SignTransaction::fixHexValue("0x076C00");
    tx.to=SignTransaction::fixHexValue(contract1.toStdString());
    tx.value=SignTransaction::fixHexValue("");
    //tx.data=SignTransaction::fixHexValue("0xa9059cbb");
    tx.data=SignTransaction::fixHexValue(data1.toStdString());
    tx.chainId = chain_id;
    //tx.chainId = 6432;
    tx.v=SignTransaction::fixHexValue(RLP::intToHex(tx.chainId));//as per EIP 155

    //std::string privkey = "1c28847b1ae871f0b4f9758a8129e4c7c09a2005ece548ca3a29382a59de6fbf";

    QString transaction = QString::fromStdString(SignTransaction::sign_transaction(tx, privkey.toStdString()));
    qDebug() << transaction;

    QJsonArray obj3;
    obj3.push_back("0x" + transaction);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_sendRawTransaction";
    obj["params"] = obj3;
    obj["id"] = 29;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    send_request.setUrl(url1);
    send_request.setRawHeader("Content-Type", "application/json");
    send_manager->post(send_request, data);
}

/*
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
*/

void Resource::managerFinished(QNetworkReply *reply) {
    try{
        if (reply->error()) {
            qDebug() << reply->errorString();
            throw;
        }

        QString answer = reply->readAll();

        if(l_json.size() > 0){
            nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());
            qDebug() << answer;

            int i = 0;
            for(;i<static_cast<int>(l_json.size())-1; i++){
                if(l_json[i] != ""){
                    if(tmp1.contains(l_json[i].toStdString())){
                        tmp1 = tmp1[l_json[i].toStdString()];
                    }
                    else{
                        throw;
                    }
                }
                else{
                    throw;
                }
            }

            double ans = static_cast<double>(tmp1[l_json[i].toStdString()]);

            d_value=ans;

            QString tmp2 = QString::number(d_value);
            int point = tmp2.indexOf('.');

            qDebug() << "point" << point;
            tmp2 = tmp2.replace(".", "");

            for(uint i=tmp2.length()-point;i<18;i++){
                tmp2 += "0";
            }

            value256 = uint256S(Util::tohex(tmp2.toStdString()));

            qDebug() << QString::fromStdString(value256.ToString());
        }
        else if(regex != ""){
            QRegularExpression re(this->regex);
            QRegularExpressionMatch match = re.match(answer);
            if (match.hasMatch()) {
                QString matched = match.captured(1);
                if(matched == ""){
                    throw;
                }
                double ans = matched.toDouble();

                QString tmp2 = QString::number(ans);
                int point = tmp2.indexOf('.');
                tmp2 = tmp2.replace(".", "");

                for(uint i=tmp2.length()-point;i<18;i++){
                    tmp2 += "0";
                }

                value256 = uint256S(Util::tohex(tmp2.toStdString()));
            }
            else{
                throw;
            }
        }
    }
    catch(...){
        qDebug() << "error json";
        *this->state=8;
        return;
    }

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
