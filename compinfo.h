#ifndef COMPINFO_H
#define COMPINFO_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <vector>

#include "data.h"


struct comp{
    QString requestID;
    QString callback;
    QString hash;
    uint block;
    QString address;
    QString chain;
    uint id;
};

class compinfo : public QObject
{
Q_OBJECT

private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QNetworkAccessManager *filter_manager;
    QNetworkRequest filter_request;

    QString filter;

    std::vector<comp> completed;

    Data *data;

    QString type;

    void create_filter_events();

public:
    compinfo(Data *_data, QString _type);
    ~compinfo();

    void update_events();
    std::vector<comp> get_completed(){return completed;};

private slots:
    void managerFinished(QNetworkReply *reply);
    void filter_managerFinished(QNetworkReply *reply);
};

#endif // COMPINFO_H
