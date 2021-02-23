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

public:
    Resource();
    Resource(QString url, std::vector<QString> _l_json, QString _contract, QString n, Data* _data);
    ~Resource();
    void update_resource();
    void send_resource();

private slots:
    void managerFinished(QNetworkReply *reply);

    void send_managerFinished(QNetworkReply *reply);
};

#endif // RESOURCE_H
