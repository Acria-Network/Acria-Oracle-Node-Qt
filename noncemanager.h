#ifndef NONCEMANAGER_H
#define NONCEMANAGER_H

#include "data.h"

#include <QNetworkAccessManager>
#include <QMutex>


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

    QMutex mutex;

    void update_nonce();
public:
    NonceManager(Data* _data, QString _type);
    ~NonceManager();
    unsigned get_nonce();
    bool is_ready(){return ready;}

    void reset();

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // NONCEMANAGER_H
