#ifndef RESOURCE_H
#define RESOURCE_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <map>
#include <QTimer>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueList>
#include <QLabel>

#include "json.hpp"
#include "uint256.h"
#include "data.h"
#include "noncemanager.h"
#include "tasks.h"


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
    std::vector<QString> l_json;
    Data *data;
    uint* state;

    QString hash;
    unsigned nonce;
    bool nonce_set;
    NonceManager* nonce_manager;
    QTimer* is_deployed_timer;
    nlohmann::json conf;
    req r;

    unsigned long long get_minimum_transaction_fee();

public:
    Resource();
    Resource(nlohmann::json _conf, req _r, Data* _data, uint* state, NonceManager* _nonce_manager);
    ~Resource();

    QString error;

    void update_resource();
    void send_resource();
    uint get_state(){return *state;}
    static QString convert_parameter(QString _parameter_type, QString _request_data);
    static QString parse_script(QString file_name, QString script_parameter, QString api_answer, bool& success, QLabel* label = NULL);

private slots:
    void managerFinished(QNetworkReply *reply);

    void send_managerFinished(QNetworkReply *reply);

    void deployed_managerFinished(QNetworkReply *reply);

    void is_deployed();
};

#endif // RESOURCE_H
