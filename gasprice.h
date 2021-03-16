#ifndef GASPRICE_H
#define GASPRICE_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <math.h>

#include "data.h"


class GasPrice : public QObject
{
Q_OBJECT

private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QString type;
    Data *data;

public:
    GasPrice(Data *_data, QString _type);
    ~GasPrice();
    void update_gas_price();

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // GASPRICE_H
