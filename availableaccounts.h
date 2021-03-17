#ifndef AVAILABLEACCOUNTS_H
#define AVAILABLEACCOUNTS_H

#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <math.h>

#include "data.h"


namespace Ui {
class AvailableAccounts;
}

class AvailableAccounts : public QDialog
{
    Q_OBJECT

public:
    explicit AvailableAccounts(QWidget *parent = nullptr, Data *_data = nullptr, QString _type = "");
    ~AvailableAccounts();

    QString selected_account;

    void update_accounts();

private:
    Ui::AvailableAccounts *ui;

    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QString type;
    Data *data;

private slots:
    void managerFinished(QNetworkReply *reply);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // AVAILABLEACCOUNTS_H
