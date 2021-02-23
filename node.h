#ifndef NODE_H
#define NODE_H

#include <QString>
#include <vector>
#include <map>

#include "json.hpp"
#include "resource.h"
#include "data.h"


class Node : public QObject
{
Q_OBJECT

public:
    Node(Data* _data);
    ~Node();
    bool createConfig(QString conf);
    bool get_status_geth();
    bool get_status_polkadot();
    bool get_status_acria();
    bool get_status_config();

private:
    QString config_location;
    nlohmann::json config;

    bool parseConfig();
    QString loadConfig();

    bool status_geth;
    bool status_polkadot;
    bool status_acria;
    bool status_config;

    QString geth_version;

    QNetworkAccessManager *status_manager;
    QNetworkRequest status_request;

    Data* data;

private slots:
    void statusManagerFinished(QNetworkReply *reply);
};

#endif // NODE_H
