#ifndef RESOURCE_H
#define RESOURCE_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <map>
#include <QTimer>

#include "json.hpp"
#include "uint256.h"
#include "data.h"
#include "noncemanager.h"


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

    QNetworkAccessManager *deployed_manager;
    QNetworkRequest deployed_request;

    QString s_value;
    double d_value;
    uint256 value256;
    QString url;
    QString url_data;
    QString parameter_type;
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
    QString hash;
    QString request_data;

    unsigned nonce;

    NonceManager* nonce_manager;

    QTimer* is_deployed_timer;

    unsigned long long get_minimum_transaction_fee();

    QString convert_parameter();

public:
    Resource();
    Resource(QString url, std::vector<QString> _l_json, QString _contract, QString n, Data* _data, QString _type, uint _id, uint* state, uint _max_gas, unsigned long long _fee, QString _request_data, QString _url_data, QString _parameter_type, NonceManager* _nonce_manager);
    ~Resource();

    QString error;

    void update_resource();
    void send_resource();
    void add_unsent_id(uint i){unsent_id.push_back(i);}
    uint get_state(){return *state;}

private slots:
    void managerFinished(QNetworkReply *reply);

    void send_managerFinished(QNetworkReply *reply);

    void deployed_managerFinished(QNetworkReply *reply);

    void is_deployed();
};

#endif // RESOURCE_H
