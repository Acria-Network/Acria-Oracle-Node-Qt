#ifndef AVAILABLEDATACONTRACTS_H
#define AVAILABLEDATACONTRACTS_H

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <math.h>
#include <QDialog>
#include <vector>

#include "data.h"
#include "qprogressindicator.h"


struct Contract{
    QString name;
    QString address;
};

namespace Ui {
class AvailableDataContracts;
}

class AvailableDataContracts : public QDialog
{
    Q_OBJECT

public:
    explicit AvailableDataContracts(QWidget *parent = nullptr, Data *_data = nullptr, QString _type = nullptr, QString _acria_main = "");
    ~AvailableDataContracts();

    void set_acria_main(QString _acria_main);

private:
    Ui::AvailableDataContracts *ui;

    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QString type;
    Data *data;

    QString acria_main;

    std::vector<Contract> contracts;

    QProgressIndicator* pi;

    void update_data_contracts();

private slots:
    void managerFinished(QNetworkReply *reply);
};

#endif // AVAILABLEDATACONTRACTS_H
