#include "reportactive.h"
#include "signtransaction.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>


ReportActive::ReportActive(Data* _data, QString _type)
{
    this->data = _data;
    this->type = _type;
}

ReportActive::~ReportActive()
{
    delete manager;
}

void ReportActive::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    qDebug() << answer;
}

void ReportActive::send(){
    QString msg = QString::number(QDateTime::currentSecsSinceEpoch());

    QJsonObject obj;
    obj["msg"] = msg;
    obj["contract"] = this->data->chain_data[this->type].contract;
    obj["type"] = this->type;

    if(data->chain_data[this->type].private_key != "")
        obj["signed_msg"] = QString::fromStdString(SignTransaction::sign_message(msg, data->chain_data[this->type].private_key.toStdString()));

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    request.setUrl(QUrl("http://127.0.0.1:8000/report_active"));
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, data);
}
