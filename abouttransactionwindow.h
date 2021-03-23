#ifndef ABOUTTRANSACTIONWINDOW_H
#define ABOUTTRANSACTIONWINDOW_H

#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <math.h>

#include "data.h"


namespace Ui {
class AboutTransactionWindow;
}

class AboutTransactionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AboutTransactionWindow(QWidget *parent = nullptr, Data *_data = nullptr, QString _type = "");
    ~AboutTransactionWindow();

    void init(QString _hash);

private:
    Ui::AboutTransactionWindow *ui;

    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QString type;
    QString hash;
    QString block_hash;
    Data *data;
    unsigned pos;

    void get_transaction_details();

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // ABOUTTRANSACTIONWINDOW_H
