#ifndef REPORTACTIVE_H
#define REPORTACTIVE_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "data.h"


class ReportActive : public QObject
{
Q_OBJECT

private:
    Data* data;
    QString type;

public:
    ReportActive(Data* _data, QString _type);
    ~ReportActive();

    void send();

    QNetworkAccessManager *manager;
    QNetworkRequest request;

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // REPORTACTIVE_H
