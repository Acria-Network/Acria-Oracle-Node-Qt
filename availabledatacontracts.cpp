#include "availabledatacontracts.h"
#include "ui_availabledatacontracts.h"
#include "util.h"

AvailableDataContracts::AvailableDataContracts(QWidget *parent, Data *_data, QString _type, QString _acria_main) :
    QDialog(parent),
    ui(new Ui::AvailableDataContracts)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
    this->acria_main = _acria_main;

    this->pi = new QProgressIndicator();
    this->ui->horizontalLayout_2->layout()->addWidget(pi);

    pi->startAnimation();
    pi->setColor(QColor::fromRgb(255,255,255));
}

AvailableDataContracts::~AvailableDataContracts()
{
    delete ui;
    delete manager;
}

void AvailableDataContracts::set_acria_main(QString _acria_main){
    qDebug() << "Available Contracts";
    this->ui->horizontalLayoutWidget_2->show();
    this->ui->label_progress_2->show();
    this->acria_main = _acria_main;
    this->update_data_contracts();
}

void AvailableDataContracts::update_data_contracts(){
    QUrl url1;
    QString contract, account;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account, &contract, &transaction_fee);

    QString data1 = "0x134dbac50000000000000000000000000000000000000000000000000000000000000000";

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, generate_rpc_call("eth_call", account, this->acria_main, data1, transaction_fee, 0, 58));
}

void AvailableDataContracts::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QString res = obj["result"].toString().remove(0, 2);

    qDebug() << res;

    if(res.length() > 2){
        std::vector<QString> inf;
        QString res1 = "";
        uint u = 0;

        for(int i = 0; i< res.length(); i++)
        {
            if(u >= 64){
                inf.push_back(res1);
                res1 = "";
                u = 0;
            }

            res1 += res.at(i);
            u++;
        }
        inf.push_back(res1);

        uint nHex = inf[1].toUInt(NULL,16);

        qDebug() << "nHex " << nHex;

        if(nHex != 0){
            uint arr_size = (inf.size()-2)/nHex;

            this->contracts.clear();

            for(uint i=2;i<inf.size();i+=arr_size){
                Contract r;

                QString tmp = "";
                for(int f = 1; f< inf[i+1].length(); f+=2){
                    tmp += QString(static_cast<char>((QString(inf[i+1].at(f-1)) + QString(inf[i+1].at(f))).toUInt(NULL,16)));
                }

                r.name = tmp.trimmed();
                r.address = "0x" + inf[i].remove(0, 24);

                qDebug() << "contract " << r.name << " " << r.address;

                this->contracts.push_back(r);
            }
        }
        else{
            this->contracts.clear();
        }
    }

    this->ui->tableWidget_comp->clear();
    while (ui->tableWidget_comp->rowCount() > 0)
    {
        ui->tableWidget_comp->removeRow(0);
    }

    this->ui->tableWidget_comp->setRowCount(contracts.size());
    this->ui->tableWidget_comp->setColumnCount(3);
    this->ui->tableWidget_comp->setColumnWidth(0, 40);
    this->ui->tableWidget_comp->setColumnWidth(1, 100);
    this->ui->tableWidget_comp->setColumnWidth(2, 250);

    if(contracts.size() != 0){
        this->ui->horizontalLayoutWidget_2->hide();
        this->ui->label_progress_2->hide();
        for (int d=contracts.size()-1; d>=0; d--){
            this->ui->tableWidget_comp->setItem( d, 0, new QTableWidgetItem(QString::number(d+1)));
            this->ui->tableWidget_comp->setItem( d, 1, new QTableWidgetItem(contracts[d].name));
            this->ui->tableWidget_comp->setItem( d, 2, new QTableWidgetItem(contracts[d].address));
        }
    }
    else{
        this->ui->horizontalLayoutWidget_2->show();
        this->ui->label_progress_2->show();
    }
}