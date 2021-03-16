#include "gasprice.h"
#include "util.h"

GasPrice::GasPrice(Data *_data, QString _type)
{
    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
}

GasPrice::~GasPrice(){
    delete manager;
    delete manager;
}

void GasPrice::update_gas_price(){
    QUrl url1;
    QString contract, account;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account, &contract, &transaction_fee);

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, generate_rpc_call("eth_gasPrice", "", "", "", 0, 0, 64));
}

void GasPrice::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString().remove(0, 2);

    qDebug() << res;
    qDebug() << answer;

    if(res.length() > 2){
        qDebug() << "gas price " << res.toULongLong(NULL,16);

        if(this->type == "ethereum")
            this->data->transaction_fee_geth = res.toULongLong(NULL,16);
        else if(this->type == "binance")
            this->data->transaction_fee_binance = res.toULongLong(NULL,16);
    }

}
