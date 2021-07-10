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

Resource::Resource(nlohmann::json _conf, req _r, Data* _data, uint* state, NonceManager* _nonce_manager) : Resource()
{
    this->data = _data;
    this->state = state;
    this->nonce_manager = _nonce_manager;
    this->conf = _conf;
    this->r = _r;


    if(conf.contains("json")){
        for(uint f = 0; f<conf["json"].size();f++){
            l_json.push_back(QString::fromStdString(conf["json"][f]));
        }
    }
    for(int i = l_json.size()-1; i>=0; i--){
        if(l_json[i] == ""){
            l_json.erase(l_json.begin() + i);
        }
        else
            break;
    }
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
    if(this->get_minimum_transaction_fee() < r.fee){
        qDebug() << "request_data: " << r.data;
        if(r.data != "")
            request.setUrl(QUrl(QString::fromStdString(conf["url_data"]).replace("%data%", this->convert_parameter(QString::fromStdString(conf["parameter_type"]), r.data))));
        else
            request.setUrl(QUrl(QString::fromStdString(conf["url"])));

        manager->get(request);
    }
    else{
        QTimer::singleShot(30000, this, &Resource::update_resource);
    }
}

void Resource::send_resource(){
    this->nonce = this->nonce_manager->get_nonce();

    QUrl url1;
    QString contract1, account1, privkey;
    unsigned long long transaction_fee = 0; unsigned chain_id = 0;

    this->data->get_chain_info(r.chain, &url1, &account1, &contract1, &transaction_fee, &privkey, &chain_id);

    privkey.remove(0, 2);

    Keccak keccak;

    QString d1 = "0x" + QString::fromStdString(keccak("fillRequest(bytes8,uint256,uint256)")).mid(0,8);
    QString d2 = Util::str2bytes8(QString::fromStdString(conf["rname"]))+"000000000000000000000000000000000000000000000000";
    QString d3 = QString::fromStdString(value256.GetHex());
    QString d4 = QString::fromStdString(Util::n2hexstr(r.id));
    for(uint i = d4.size(); i<64;i++){
        d4 = "0" + d4;
    }

    QString data1 = d1+d2+d3+d4;

    Transaction tx;
    std::string nonce_ = RLP::intToHex(nonce);
    if(nonce == 0)
        nonce_ = "";
    tx.nonce=SignTransaction::fixHexValue(nonce_);
    tx.gasPrice=SignTransaction::fixHexValue(RLP::intToHex(transaction_fee));
    tx.gasLimit=SignTransaction::fixHexValue("0x076C00");
    tx.to=SignTransaction::fixHexValue(contract1.toStdString());
    tx.value=SignTransaction::fixHexValue("");
    tx.data=SignTransaction::fixHexValue(data1.toStdString());
    tx.chainId = chain_id;
    tx.v=SignTransaction::fixHexValue(RLP::intToHex(tx.chainId));//as per EIP 155

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

QString Resource::parse_script(QString file_name, QString script_parameter, QString api_answer, bool& success, QLabel* label){
    QString script = Util::read_file(file_name.toStdString());
    QString bn = Util::read_file("scripts/bignumber.min.js");
    QJSEngine engine;
    QJSValue function_js = engine.evaluate(bn+"(function(api_response, arg1) {" + script + "})");
    QJSValueList args;
    args << api_answer << script_parameter;
    QJSValue result = function_js.call(args);
    qDebug() << "parsed Script " + file_name;

    if (result.isError()){
        qDebug() << "error Script";
        if(label != NULL)
            label->setText(tr("Uncaught exception at line ") + result.property("lineNumber").toString() + ":" + result.toString());
        success = false;
        return "";
    }
    else{
        QString tmp2;
        uint256 value256;
        if(result.isNumber()){
            tmp2 = Util::double2uint256(result.toNumber());
            value256 = uint256S(Util::tohex(tmp2.toStdString()));
        }
        else if(result.isString()){
            if (QRegExp("^0x[A-Fa-f0-9]+$").exactMatch(result.toString()))
               tmp2 = result.toString();
            else
               tmp2 = Util::str2bytes32(result.toString());
            value256 = uint256S(tmp2.toStdString());
        }
        else if(result.isUndefined()){
            if(label != NULL)
                label->setText("Undefined Value");
            success = false;
            return "";
        }

        success = true;
        if(label != NULL)
            label->setText("Value: " + result.toString()+"\nuint256: "+tmp2+"\nuint256(hex): "+QString::fromStdString(value256.GetHex()));

        return QString::fromStdString(value256.GetHex());
    }
}

void Resource::managerFinished(QNetworkReply *reply) {
    try{
        if (reply->error()) {
            qDebug() << reply->errorString();
            throw;
        }

        QString answer = reply->readAll();
        qDebug() << answer;

        if(l_json.size() > 0){
            nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());

            int i = 0;
            for(;i<static_cast<int>(l_json.size())-1; i++){
                if(l_json[i] != ""){
                    if(tmp1.contains(l_json[i].toStdString()))
                        tmp1 = tmp1[l_json[i].toStdString()];
                    else
                        throw;
                }
                else
                    throw;
            }

            d_value = static_cast<double>(tmp1[l_json[i].toStdString()]);
            QString tmp2 = Util::double2uint256(d_value);
            value256 = uint256S(Util::tohex(tmp2.toStdString()));

            qDebug() << "Resource to send: " << QString::fromStdString(value256.ToString());
        }
        else if(conf.contains("regex")){
            QRegularExpression re(QString::fromStdString(conf["regex"]));
            QRegularExpressionMatch match = re.match(answer);
            if (match.hasMatch()) {
                QString matched = match.captured(1);
                if(matched == "")
                    throw;

                d_value = matched.toDouble();
                QString tmp2 = Util::double2uint256(d_value);
                value256 = uint256S(Util::tohex(tmp2.toStdString()));

                qDebug() << "Resource to send: " << QString::fromStdString(value256.ToString());
            }
            else
                throw;
        }
        else if(conf.contains("script_file")){
            bool script_success = false;
            value256 = uint256S(Resource::parse_script(QString::fromStdString(conf["script_file"]), QString::fromStdString(conf["script_parameter"]), answer, script_success).toStdString());

            qDebug() << "Resource to send: " << QString::fromStdString(value256.ToString());
        }
    }
    catch(...){
        qDebug() << "error json";
        *this->state=8;
        QTimer::singleShot(30000, this, &Resource::update_resource);
        return;
    }

    send_resource();
}

unsigned long long Resource::get_minimum_transaction_fee(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(r.chain, &url1, &account1, &contract1, &transaction_fee);

    unsigned long long total_gas = r.max_gas + 40000;

    unsigned long long mfee = total_gas*transaction_fee;
    qDebug() << "minimum fee " << mfee;
    qDebug() << "paid fee " << r.fee;

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
    is_deployed_timer->start(10000);
}

void Resource::is_deployed(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(r.chain, &url1, &account1, &contract1, &transaction_fee);

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
