#ifndef SIGNCONFIGWINDOW_H
#define SIGNCONFIGWINDOW_H

#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "data.h"

namespace Ui {
class SignConfigWindow;
}

class SignConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SignConfigWindow(QWidget *parent = nullptr, Data* _data = nullptr);
    ~SignConfigWindow();

    void sign_all(QString config);

private slots:
    void on_pushButton_upload_tooracle_marketplace_clicked();

    void managerFinished(QNetworkReply *reply);

private:
    Ui::SignConfigWindow *ui;

    Data* data;

    QNetworkAccessManager *manager;
    QNetworkRequest request;
};

#endif // SIGNCONFIGWINDOW_H
