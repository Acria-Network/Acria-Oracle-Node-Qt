#ifndef DATA_H
#define DATA_H

#include <QString>
#include <vector>
#include <QUrl>
#include <map>

struct ChainData{
    QString url;
    QString account;
    QString private_key;
    QString contract;
    QString wallet_path;
    unsigned chain_id;
    unsigned chain_id_given;
    bool custom_chain_id_enabled;
    bool enabled;
    unsigned long long transaction_fee;
    std::vector<QString> items;
    __uint128_t balance;
};


class Data
{
public:
    Data();
    std::map<QString, ChainData> chain_data;
    std::vector<QString> chains;

    bool changed = false;

    void load_settings();
    void save_settings();

    void get_chain_info(QString chain, QUrl* url, QString* account, QString* contract, unsigned long long* transaction_fee, QString* privkey = NULL, unsigned* chain_id = NULL);

    void items_clear(QString _type){
        this->chain_data[_type].items.clear();
    }

    void item_push_back(QString _type, QString item){
        this->chain_data[_type].items.push_back(item.trimmed());
    }
};

#endif // DATA_H
