#ifndef NONCEMANAGER_H
#define NONCEMANAGER_H

#include "data.h"

#include <QNetworkAccessManager>


class NonceManager : public QObject
{
Q_OBJECT

private:
    unsigned nonce;
    bool ready;
    QString type;
    Data* data;

    QNetworkAccessManager *manager;
    QNetworkRequest request;

    void update_nonce();
public:
    NonceManager(Data* _data, QString _type);
    unsigned get_nonce();

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // NONCEMANAGER_H
