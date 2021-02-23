#ifndef DATA_H
#define DATA_H

#include <QString>

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

    void load_settings();
    void save_settings();
};

#endif // DATA_H
