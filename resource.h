#ifndef RESOURCE_H
#define RESOURCE_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <map>

#include "json.hpp"
#include "uint256.h"
#include "data.h"


struct Source{
    QString location;
    std::vector<QString> l_json;
};

class Resource : public QObject
{
Q_OBJECT

private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QNetworkAccessManager *send_manager;
    QNetworkRequest send_request;

    QString s_value;
    double d_value;
    uint256 value256;
    QString url;
    QString contract;
    QString item;
    std::vector<QString> l_json;
    Data *data;

    QString type;

    std::vector<uint> sent_id;
    std::vector<uint> unsent_id;
    uint* state;

    uint id;
    unsigned long long fee;
    uint max_gas;

    unsigned long long get_minimum_transaction_fee();

public:
    Resource();
    Resource(QString url, std::vector<QString> _l_json, QString _contract, QString n, Data* _data, QString _type, uint _id, uint* state, uint _max_gas, unsigned long long _fee);
    ~Resource();
    void update_resource();
    void send_resource();
    void add_unsent_id(uint i){unsent_id.push_back(i);}
    uint get_state(){return *state;}

private slots:
    void managerFinished(QNetworkReply *reply);

    void send_managerFinished(QNetworkReply *reply);
};

#endif // RESOURCE_H
