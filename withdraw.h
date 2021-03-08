#ifndef WITHDRAW_H
#define WITHDRAW_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <math.h>

#include "data.h"
#include "processingwindow.h"


class Withdraw : public QObject
{
Q_OBJECT

private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QString type;
    Data *data;

    ProcessingWindow* processing_window;

public:
    Withdraw(Data *_data, QString _type);
    ~Withdraw();

    void withdraw(ProcessingWindow* _processing_window);

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // WITHDRAW_H
