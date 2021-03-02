#ifndef DATA_H
#define DATA_H

#include <QString>
#include <vector>


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

    void load_settings();
    void save_settings();
};

#endif // DATA_H
