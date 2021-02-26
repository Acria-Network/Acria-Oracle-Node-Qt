#ifndef TASKS_H
#define TASKS_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "data.h"


struct req{
    QString requestID;
    bool cancelled;
    uint expiration;
    QString callback;
};

class Tasks : public QObject
{
Q_OBJECT

private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QNetworkAccessManager *r_manager;
    QNetworkRequest r_request;

    std::vector<req> requests;

    Data *data;

public:
    Tasks(Data *_data);
    ~Tasks();

    void update_tasks();
    void update_requests();

    std::vector<req> get_requests(){return requests;};

private slots:
    void managerFinished(QNetworkReply *reply);
    void r_managerFinished(QNetworkReply *reply);

};

#endif // TASKS_H
