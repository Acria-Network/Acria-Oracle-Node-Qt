#ifndef BALANCES_H
#define BALANCES_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <math.h>

#include "data.h"


class Balances : public QObject
{
Q_OBJECT

private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QString type;
    Data *data;

    unsigned long long withdrawable;

public:
    Balances(Data *_data, QString _type);
    ~Balances();

    void update_withdrawable();

    double get_withdrawable(){return double(withdrawable / pow(10,18));}

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // BALANCES_H
