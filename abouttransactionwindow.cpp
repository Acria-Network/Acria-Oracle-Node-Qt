#include "abouttransactionwindow.h"
#include "ui_abouttransactionwindow.h"
#include "util.h"
#include "json.hpp"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonArray>
#include <math.h>
#include <string>


AboutTransactionWindow::AboutTransactionWindow(QWidget *parent, Data *_data, QString _type) :
    QDialog(parent),
    ui(new Ui::AboutTransactionWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;

    this->ui->tableWidget->setRowCount(9);
    this->ui->tableWidget->setColumnCount(2);
    this->ui->tableWidget->setColumnWidth(0, 120);

    this->pos = 0;
}

AboutTransactionWindow::~AboutTransactionWindow()
{
    delete ui;
    delete manager;
}

void AboutTransactionWindow::init(QString _hash){
    this->pos = 0;
    this->hash = _hash;

    get_transaction_details();
}

void AboutTransactionWindow::get_transaction_details(){
    QUrl url1;
    QString contract1, account1;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account1, &contract1, &transaction_fee);

    QJsonArray obj3;
    if(this->pos < 2)
        obj3.push_back(this->hash);
    else{
        obj3.push_back(this->block_hash);
        obj3.push_back(true);
    }

    QJsonObject obj;
    obj["jsonrpc"] = "2.0";
    if(this->pos == 0)
        obj["method"] = "eth_getTransactionReceipt";
    else if(this->pos == 1)
        obj["method"] = "eth_getTransactionByHash";
    else
        obj["method"] = "eth_getBlockByHash";
    obj["params"] = obj3;
    obj["id"] = 16;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, data);
}

void AboutTransactionWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error AboutTransactionWindow (" << this->type << "): " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());

    qDebug() << "AboutTransactionWindow: " << answer;

    if(tmp1["result"]["blockNumber"] != nullptr || tmp1["result"]["hash"] != nullptr){
        if(this->pos == 0){
            this->ui->tableWidget->setItem( 0, 0, new QTableWidgetItem("Transaction Hash:"));
            this->ui->tableWidget->setItem( 0, 1, new QTableWidgetItem(this->hash));

            this->ui->tableWidget->setItem( 1, 0, new QTableWidgetItem("Status:"));
            this->ui->tableWidget->setItem( 1, 1, new QTableWidgetItem(tmp1["result"]["status"]=="0x1"?"Success":"Error"));

            this->ui->tableWidget->setItem( 2, 0, new QTableWidgetItem("Block:"));
            this->ui->tableWidget->setItem( 2, 1, new QTableWidgetItem(QString::number(QString::fromStdString(tmp1["result"]["blockNumber"]).toUInt(NULL, 16))));

            this->ui->tableWidget->setItem( 4, 0, new QTableWidgetItem("From:"));
            this->ui->tableWidget->setItem( 4, 1, new QTableWidgetItem(QString::fromStdString(tmp1["result"]["from"])));

            this->ui->tableWidget->setItem( 5, 0, new QTableWidgetItem("To:"));
            this->ui->tableWidget->setItem( 5, 1, new QTableWidgetItem(QString::fromStdString(tmp1["result"]["to"])));

            this->ui->tableWidget->setItem( 7, 0, new QTableWidgetItem("Gas Used:"));
            this->ui->tableWidget->setItem( 7, 1, new QTableWidgetItem(QString::number(QString::fromStdString(tmp1["result"]["gasUsed"]).toUInt(NULL, 16))));

            this->pos++;
            get_transaction_details();
        }
        else if(this->pos == 1){
            this->ui->tableWidget->setItem( 6, 0, new QTableWidgetItem("Gas Price:"));
            this->ui->tableWidget->setItem( 6, 1, new QTableWidgetItem(QString::number(QString::fromStdString(tmp1["result"]["gasPrice"]).toULongLong(NULL, 16)/1000000000)));

            this->ui->tableWidget->setItem( 8, 0, new QTableWidgetItem("Nonce:"));
            this->ui->tableWidget->setItem( 8, 1, new QTableWidgetItem(QString::number(QString::fromStdString(tmp1["result"]["nonce"]).toUInt(NULL, 16))));

            this->block_hash = QString::fromStdString(tmp1["result"]["blockHash"]);

            this->pos++;
            get_transaction_details();
        }
        else{
            QDateTime timestamp;
            timestamp.setTime_t(QString::fromStdString(tmp1["result"]["timestamp"]).toUInt(NULL, 16));

            this->ui->tableWidget->setItem( 3, 0, new QTableWidgetItem("Timestamp:"));
            this->ui->tableWidget->setItem( 3, 1, new QTableWidgetItem(timestamp.toString(Qt::SystemLocaleShortDate)));
        }
        this->ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    }
    else{
        qDebug() << "hash - not deployed";
    }
}
