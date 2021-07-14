#include "withdraw.h"
#include "processingwindow.h"
#include "util.h"
#include "signtransaction.h"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>

Withdraw::Withdraw(Data *_data, QString _type, NonceManager* _nonce_manager)
{
    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
    this->nonce_manager = _nonce_manager;
}

Withdraw::~Withdraw(){
    delete manager;
}

void Withdraw::withdraw(ProcessingWindow* _processing_window){
    this->processing_window = _processing_window;

    QUrl url1;
    QString contract1, account1, privkey;
    unsigned long long transaction_fee = 0; unsigned chain_id;
    unsigned nonce = this->nonce_manager->get_nonce();

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee, &privkey, &chain_id);
    privkey.remove(0, 2);

    QString d1 = "0x3ccfd60b";

    Transaction tx;

    std::string nonce_ = RLP::intToHex(nonce);
    if(nonce == 0)
        nonce_ = "";
    tx.nonce=SignTransaction::fixHexValue(nonce_);
    tx.gasPrice=SignTransaction::fixHexValue(RLP::intToHex(transaction_fee));
    tx.gasLimit=SignTransaction::fixHexValue(RLP::intToHex(101264));
    tx.to=SignTransaction::fixHexValue(contract1.toStdString());
    tx.value=SignTransaction::fixHexValue("");
    tx.data=SignTransaction::fixHexValue(d1.toStdString());
    //tx.chainId = 6432;
    tx.chainId = chain_id;
    tx.v=SignTransaction::fixHexValue(RLP::intToHex(tx.chainId));//as per EIP 155

    QString transaction = QString::fromStdString(SignTransaction::sign_transaction(tx, privkey.toStdString()));

    QJsonArray obj3;
    obj3.push_back("0x" + transaction);

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "eth_sendRawTransaction";
    obj["params"] = obj3;
    obj["id"] = 31;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, data);
}

void Withdraw::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = Util::ObjectFromString(answer);
    QString res = obj["result"].toString();

    qDebug() << answer;

    this->processing_window->completed(res);
}
