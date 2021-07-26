#ifndef TASKS_H
#define TASKS_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "data.h"


struct req{
    QString requestID;
    unsigned long long fee;
    uint expiration;
    QString callback;
    QString chain;
    uint id;
    uint max_gas;
    QString data;
};

class Tasks : public QObject
{
Q_OBJECT

private:
    QNetworkAccessManager *r_manager;
    QNetworkRequest r_request;

    std::vector<req> requests;

    Data *data;

    QString type;

public:
    Tasks(Data *_data, QString _type);
    ~Tasks();

    void update_tasks();
    void update_requests();

    std::vector<req> get_requests(){return requests;};

private slots:
    void r_managerFinished(QNetworkReply *reply);

};

#endif // TASKS_H
