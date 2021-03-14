#ifndef DEPLOYWINDOW_H
#define DEPLOYWINDOW_H

#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <math.h>

#include "data.h"
#include "processingwindow.h"

namespace Ui {
class DeployWindow;
}

class DeployWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DeployWindow(QWidget *parent = nullptr, Data *_data = nullptr, QString _type = nullptr, ProcessingWindow* _processing_window = nullptr, QString _hash1 = "");
    ~DeployWindow();

    void deploy();

private slots:
    void managerFinished(QNetworkReply *reply);
    void deployed_managerFinished(QNetworkReply *reply);

    void on_buttonBox_accepted();
    void is_deployed();

private:
    Ui::DeployWindow *ui;

    QNetworkAccessManager *manager;
    QNetworkRequest request;

    QNetworkAccessManager *deployed_manager;
    QNetworkRequest deployed_request;

    QString type;
    Data *data;

    ProcessingWindow* processing_window;

    uint state;
    QString hash1;
};

#endif // DEPLOYWINDOW_H
