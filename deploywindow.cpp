#include "deploywindow.h"
#include "ui_deploywindow.h"
#include "util.h"
#include "json.hpp"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>

DeployWindow::DeployWindow(QWidget *parent, Data *_data, QString _type, ProcessingWindow* _processing_window, QString _hash1, NonceManager* _nonce_manager) :
    QDialog(parent),
    ui(new Ui::DeployWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    deployed_manager = new QNetworkAccessManager();

    QObject::connect(deployed_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(deployed_managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
    this->state = 3;
    this->hash1 = _hash1;

    this->processing_window = _processing_window;

    this->nonce_manager = _nonce_manager;

    this->available_data_contracts = new AvailableDataContracts(this, this->data, this->type, "");
}

DeployWindow::~DeployWindow()
{
    delete ui;
    delete manager;
    delete available_data_contracts;
}

void DeployWindow::deploy(){
    if(this->state == 3){
        this->state = 0;

        QUrl url1;
        QString contract1, account1;
        unsigned long long transaction_fee = 0;

        this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

        QString d1 = "0x1cd23fd9";
        QString d2 = Util::str2bytes32(this->ui->lineEdit_contract_name->text());
        QString data1 = d1+d2;

        request.setUrl(url1);
        request.setRawHeader("Content-Type", "application/json");
        manager->post(request, Util::generate_rpc_call("eth_sendTransaction", account1, this->ui->lineEdit_main_contract->text().trimmed(), data1, transaction_fee, 1601264, 25, this->nonce_manager->get_nonce()));

        this->state=1;
    }
}

void DeployWindow::is_deployed(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    QString d1 = "0x3f83acff";
    QString d2 = Util::str2bytes32(this->ui->lineEdit_contract_name->text());
    QString data1 = d1+d2;

    deployed_request.setUrl(url1);
    deployed_request.setRawHeader("Content-Type", "application/json");
    deployed_manager->post(deployed_request, Util::generate_rpc_call("eth_call", account1, this->ui->lineEdit_main_contract->text(), data1, transaction_fee, 0, 79, -1));
}

void DeployWindow::handle_error(QString err){
    this->processing_window->completed(err, true);
    this->state=3;
}

void DeployWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        this->handle_error(reply->errorString());
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = Util::ObjectFromString(answer);
    if(obj.contains("error")){
        qDebug() << "error deploy";
        qDebug() << answer;
        nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());
        this->handle_error(QString::fromStdString(tmp1["error"]["message"].dump()));
        return;
    }
    QString res = obj["result"].toString();

    qDebug() << answer;

    this->state=2;

    this->hash1 = res;
    this->is_deployed();
}

void DeployWindow::deployed_managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = Util::ObjectFromString(answer);
    qDebug() << answer;

    if(obj.contains("error")){
        qDebug() << "error deploy";
        nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());
        this->handle_error(QString::fromStdString(tmp1["error"]["message"].dump()));
        return;
    }
    QString res = obj["result"].toString();

    qDebug() << res;
    if(res == "0x"){
        qDebug() << "error deploy 0x";
        this->handle_error("Error: potentially wrong contract address");
        return;
    }
    if(res != "0x0000000000000000000000000000000000000000000000000000000000000000"){
        this->processing_window->completed("0x" + res.remove(0, 26));
        this->state=3;
    }
    else{
        this->is_deployed();
    }
}

void DeployWindow::on_buttonBox_accepted()
{
    if(this->state == 3){
        this->processing_window->show();
        this->deploy();
    }
}

void DeployWindow::on_pushButton_data_contracts_clicked()
{
    this->available_data_contracts->set_acria_main(this->ui->lineEdit_main_contract->text());
    this->available_data_contracts->exec();
}

void DeployWindow::on_lineEdit_contract_name_textChanged(const QString &arg1)
{
    if(arg1.length()>32){
        static_cast<QLineEdit*>(sender())->setText(arg1.mid(0, 32));
    }
}
