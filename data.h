#ifndef DATA_H
#define DATA_H

#include <QString>
#include <vector>
#include <QUrl>


class Data
{
public:
    Data();
    QString geth_url;
    QString eth_account;
    QString eth_contract;

    QString polkadot_url;
    QString polkadot_account;
    QString polkadot_contract;

    QString binance_url;
    QString binance_account;
    QString binance_contract;

    std::vector<QString> items;
    std::vector<QString> binance_items;
    std::vector<QString> polkadot_items;

    unsigned long long transaction_fee_geth;
    unsigned long long transaction_fee_binance;

    void load_settings();
    void save_settings();

    void get_chain_info(QString chain, QUrl* url, QString* account, QString* contract, unsigned long long* transaction_fee);

    void items_clear(QString _type){
        if(_type == "ethereum")
            this->items.clear();
        else if(_type == "binance")
            this->binance_items.clear();
    }

    void item_push_back(QString _type, QString item){
        if(_type == "ethereum")
            this->items.push_back(item.trimmed());
        else if(_type == "binance")
            this->binance_items.push_back(item.trimmed());
    }
};

#endif // DATA_H
