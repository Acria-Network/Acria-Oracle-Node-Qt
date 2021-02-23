#include "node.h"
#include "util.h"

#include <QString>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>


bool Node::createConfig(QString conf){
    QString filename=this->config_location;
    QFile file(filename);

    if(!file.exists()){
        qDebug() << "Config File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
    }

   if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out << conf;
        file.close();
   }

   return this->parseConfig();
}

QString Node::loadConfig(){
    QString filename=this->config_location;
    QFile file(filename);

    if(!file.exists()){
        qDebug() << "Config File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
    }

    QString line;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line += stream.readLine();
            qDebug() << "line: "<<line;
        }
    }
    file.close();

    return line;
}

bool Node::parseConfig(){
    config = nlohmann::json::parse(this->loadConfig().toStdString());

    return true;
}

Node::Node(Data* _data)
{
    this->data = _data;

    config_location = "config.conf";

    status_manager = new QNetworkAccessManager();

    QObject::connect(status_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(statusManagerFinished(QNetworkReply*)));

    this->status_geth = false;
    this->status_polkadot = true;
    this->status_acria = true;
    this->status_config = true;

    qDebug() << "answer";
    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    obj["method"] = "web3_clientVersion";
    obj["params"] = "";
    obj["id"] = 67;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    status_request.setUrl(QUrl(this->data->geth_url));
    status_request.setRawHeader("Content-Type", "application/json");
    status_manager->post(status_request, data);
};

Node::~Node()
{
    delete status_manager;
};

void Node::statusManagerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "error " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    qDebug() << "answer " <<answer;

    QJsonObject obj = ObjectFromString(answer);

    qDebug() << obj["result"].toString();

    this->geth_version = obj["result"].toString();

    status_geth = true;
}

bool Node::get_status_geth(){
    return status_geth;
}

bool Node::get_status_polkadot(){
    return status_polkadot;
}

bool Node::get_status_acria(){
    return status_acria;
}

bool Node::get_status_config(){
    return status_config;
}
