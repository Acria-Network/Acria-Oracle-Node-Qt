#include "data.h"
#include "json.hpp"

#include <QFile>
#include <QDebug>
#include <QUrl>


Data::Data()
{
    //this->transaction_fee_geth = 20000000000;
    //this->transaction_fee_binance = 20000000000;

    this->transaction_fee_geth = 0;
    this->transaction_fee_binance = 0;
}

void Data::save_settings(){
    nlohmann::json tmp;

    tmp["geth_url"] = geth_url.toStdString();
    tmp["eth_account"] = eth_account.toStdString();
    tmp["eth_contract"] = eth_contract.toStdString();

    tmp["polkadot_url"] = polkadot_url.toStdString();
    tmp["polkadot_account"] = polkadot_account.toStdString();
    tmp["polkadot_contract"] = polkadot_contract.toStdString();

    tmp["binance_url"] = binance_url.toStdString();
    tmp["binance_account"] = binance_account.toStdString();
    tmp["binance_contract"] = binance_contract.toStdString();

    QString filename="settings.conf";
    QFile file(filename);

    if(!file.exists()){
        qDebug() << "Settings File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
    }

   if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out << QString::fromStdString(tmp.dump());
        file.close();
   }
}

void Data::load_settings(){
    QString filename="settings.conf";
    QFile file(filename);

    if(!file.exists()){
        qDebug() << "Settings File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
    }

    QString line;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line += stream.readLine();
            qDebug() << "line: "<<line;
        }
    }
    file.close();

    nlohmann::json tmp = nlohmann::json::parse(line.toStdString());

    try{
        geth_url = QString::fromStdString(tmp["geth_url"]);
        eth_account = QString::fromStdString(tmp["eth_account"]);
        eth_contract = QString::fromStdString(tmp["eth_contract"]);
    }
    catch(...){
        qDebug() << "Error settings ethereum";
    }

    try{
        polkadot_url = QString::fromStdString(tmp["polkadot_url"]);
        polkadot_account = QString::fromStdString(tmp["polkadot_account"]);
        polkadot_contract = QString::fromStdString(tmp["polkadot_contract"]);
    }
    catch(...){
        qDebug() << "Error settings polkadot";
    }

    try{
        binance_url = QString::fromStdString(tmp["binance_url"]);
        binance_account = QString::fromStdString(tmp["binance_account"]);
        binance_contract = QString::fromStdString(tmp["binance_contract"]);
    }
    catch(...){
        qDebug() << "Error settings binance";
    }
}

void Data::get_chain_info(QString chain, QUrl* url, QString* account, QString* contract, unsigned long long* transaction_fee){
    if(chain == "ethereum"){
        *url = QUrl(this->geth_url);
        *contract = this->eth_contract;
        *account = this->eth_account;
        *transaction_fee = this->transaction_fee_geth;
    }

    else if(chain == "binance"){
        *url = QUrl(this->binance_url);
        *contract = this->binance_contract;
        *account = this->binance_account;
        *transaction_fee = this->transaction_fee_binance;
    }
}
