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
    __uint128_t balance;
};


class Data
{
public:
    Data();
    std::map<QString, ChainData> chain_data;
    std::vector<QString> chains;

    bool changed = true;

    void load_settings();
    void save_settings();

    void get_chain_info(QString chain, QUrl* url, QString* account, QString* contract, unsigned long long* transaction_fee, QString* privkey = NULL, unsigned* chain_id = NULL);
};

#endif // DATA_H
