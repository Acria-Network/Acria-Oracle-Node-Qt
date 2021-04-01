#ifndef DATA_H
#define DATA_H

#include <QString>
#include <vector>
#include <QUrl>
#include <map>


class Data
{
public:
    Data();
    QString geth_url;
    QString eth_account;
    QString eth_private_key;
    QString eth_contract;
    QString eth_wallet_path;
    unsigned eth_chain_id;
    bool eth_enabled;

    QString polkadot_url;
    QString polkadot_account;
    QString polkadot_private_key;
    QString polkadot_contract;
    QString polkadot_wallet_path;
    bool polkadot_enabled;

    QString binance_url;
    QString binance_account;
    QString binance_private_key;
    QString binance_contract;
    QString binance_wallet_path;
    unsigned binance_chain_id;
    bool binance_enabled;

    bool changed = false;

    std::vector<QString> items;
    std::vector<QString> binance_items;
    std::vector<QString> polkadot_items;

    unsigned long long transaction_fee_geth;
    unsigned long long transaction_fee_binance;

    void load_settings();
    void save_settings();

    void get_chain_info(QString chain, QUrl* url, QString* account, QString* contract, unsigned long long* transaction_fee, QString* privkey = NULL, unsigned* chain_id = NULL);

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
