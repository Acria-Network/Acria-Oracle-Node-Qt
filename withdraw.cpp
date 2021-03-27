#include "withdraw.h"
#include "processingwindow.h"
#include "util.h"

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
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    QString d1 = "0x3ccfd60b";
    qDebug() << d1;

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, Util::generate_rpc_call("eth_sendTransaction", account1, contract1, d1, transaction_fee, 486400, 11, this->nonce_manager->get_nonce()));
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
