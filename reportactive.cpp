#include "reportactive.h"
#include "signtransaction.h"
#include "util.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>


ReportActive::ReportActive(Data* _data, QString _type)
{
    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
}

ReportActive::~ReportActive()
{
    delete manager;
}

void ReportActive::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error reporting Node as active: " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    qDebug() << "Reported Node as active: " << answer;
}

void ReportActive::send(){
    QString msg = QString::number(QDateTime::currentSecsSinceEpoch());

    QJsonObject obj;
    obj["msg"] = msg;
    obj["contract"] = this->data->chain_data[this->type].contract;
    obj["type"] = this->type;

    if(data->chain_data[this->type].private_key != "")
        obj["signed_msg"] = QString::fromStdString(SignTransaction::sign_message(msg, data->chain_data[this->type].private_key.toStdString()));
    else
        return;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    request.setUrl(QUrl(URL_MARKETPLACE + "/report_active"));
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, data);
}
