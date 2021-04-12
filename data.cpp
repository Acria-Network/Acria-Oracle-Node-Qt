#include "data.h"
#include "json.hpp"

#include <QFile>
#include <QDebug>
#include <QUrl>


Data::Data()
{
    this->chains = {"ethereum", "binance", "polkadot"};

    for(unsigned i = 0; i < this->chains.size(); i++){
        ChainData c;
        c.transaction_fee = 0;
        c.chain_id = 0;
        c.enabled = 0;
        c.balance = 0;
        c.chain_id_given = 0;
        c.custom_chain_id_enabled = false;
        chain_data[chains[i]] = c;
    }
}

void Data::save_settings(){
    nlohmann::json tmp;

    for(unsigned i = 0; i < this->chains.size(); i++){
        tmp[chains[i].toStdString() + "_url"] = chain_data[chains[i]].url.toStdString();
        tmp[chains[i].toStdString() + "_account"] = chain_data[chains[i]].account.toStdString();
        tmp[chains[i].toStdString() + "_contract"] = chain_data[chains[i]].contract.toStdString();
        tmp[chains[i].toStdString() + "_enabled"] = chain_data[chains[i]].enabled;
        tmp[chains[i].toStdString() + "_wallet_path"] = chain_data[chains[i]].wallet_path.toStdString();
        tmp[chains[i].toStdString() + "_chain_id_given"] = chain_data[chains[i]].chain_id_given;
        tmp[chains[i].toStdString() + "_custom_chain_id_enabled"] = chain_data[chains[i]].custom_chain_id_enabled;
    }

    QString filename="settings.conf";
    QFile file(filename);

    if(file.exists()){
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

    if(file.exists()){
        qDebug() << "Settings File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
        return;
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

    try{
        nlohmann::json tmp = nlohmann::json::parse(line.toStdString());

        for(unsigned i = 0; i < this->chains.size(); i++){
            try{
                chain_data[chains[i]].url = QString::fromStdString(tmp[chains[i].toStdString() + "_url"]);
                chain_data[chains[i]].account = QString::fromStdString(tmp[chains[i].toStdString() + "_account"]);
                chain_data[chains[i]].contract = QString::fromStdString(tmp[chains[i].toStdString() + "_contract"]);
                chain_data[chains[i]].enabled = tmp[chains[i].toStdString() + "_enabled"];
                chain_data[chains[i]].wallet_path = QString::fromStdString(tmp[chains[i].toStdString() + "_wallet_path"]);
                chain_data[chains[i]].chain_id_given = tmp[chains[i].toStdString() + "_chain_id_given"];
                chain_data[chains[i]].custom_chain_id_enabled = tmp[chains[i].toStdString() + "_custom_chain_id_enabled"];
            }
            catch(...){
                qDebug() << "Error settings " + chains[i];
            }
        }
    }
    catch(...){
        qDebug() << "Error parsing settings file";
    }
}

void Data::get_chain_info(QString chain, QUrl* url, QString* account, QString* contract, unsigned long long* transaction_fee, QString* privkey, unsigned* chain_id){
    *url = QUrl(this->chain_data[chain].url);
    *contract = this->chain_data[chain].contract;
    *account = this->chain_data[chain].account;
    *transaction_fee = this->chain_data[chain].transaction_fee;
    if(privkey != NULL)
        *privkey = this->chain_data[chain].private_key;
    if(chain_id != NULL){
        if(this->chain_data[chain].custom_chain_id_enabled)
            *chain_id = this->chain_data[chain].chain_id_given;
        else
            *chain_id = this->chain_data[chain].chain_id;
    }
}
