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
    Node(Data* _data, QString _type);
    ~Node();
    bool get_status_geth();

    void update_geth_status();

    QString get_geth_version(){return geth_version;}

private:
    bool status_geth;
    bool status_geth_chain_id = false;

    QString geth_version;

    QNetworkAccessManager *status_manager;
    QNetworkRequest status_request;

    QNetworkAccessManager *chain_id_manager;
    QNetworkRequest chain_id_request;

    Data* data;

    QString type;

    unsigned chain_id = 0;

private slots:
    void statusManagerFinished(QNetworkReply *reply);
    void chain_idManagerFinished(QNetworkReply *reply);
};

#endif // NODE_H
