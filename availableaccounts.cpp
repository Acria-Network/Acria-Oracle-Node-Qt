#include "availableaccounts.h"
#include "ui_availableaccounts.h"
#include "util.h"

AvailableAccounts::AvailableAccounts(QWidget *parent, Data *_data, QString _type) :
    QDialog(parent),
    ui(new Ui::AvailableAccounts)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    this->data = _data;
    this->type = _type;
}

AvailableAccounts::~AvailableAccounts()
{
    delete ui;
    delete manager;
}

void AvailableAccounts::update_accounts(){
    QUrl url1;
    QString contract, account;
    unsigned long long transaction_fee = 0;

    this->data->get_chain_info(this->type, &url1, &account, &contract, &transaction_fee);

    request.setUrl(url1);
    request.setRawHeader("Content-Type", "application/json");
    manager->post(request, generate_rpc_call("eth_accounts", "", "", "", 0, 0, 94, -1));
}

void AvailableAccounts::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = ObjectFromString(answer);
    QJsonArray res = obj["result"].toArray();

    qDebug() << res;
    qDebug() << answer;

    this->ui->tableWidget->clear();
    while (ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }

    this->ui->tableWidget->setRowCount(res.size());
    this->ui->tableWidget->setColumnCount(2);

    this->ui->tableWidget->setColumnWidth(0, 30);
    this->ui->tableWidget->setColumnWidth(1, 250);

    if(res.size() > 0){
        for(int i = 0;i<res.size();i++){
            qDebug() << "available account " << res[i].toString();

            this->ui->tableWidget->setItem( i, 0, new QTableWidgetItem(QString::number(i+1)));
            this->ui->tableWidget->setItem( i, 1, new QTableWidgetItem(res[i].toString()));
        }
    }

}

void AvailableAccounts::on_buttonBox_accepted()
{
    this->selected_account = "";
    QItemSelectionModel *select = this->ui->tableWidget->selectionModel();

    if(select->hasSelection()){
        qDebug() << "selected account " << select->selectedRows().at(0).siblingAtColumn(1).data().toString();

        this->selected_account = select->selectedRows().at(0).siblingAtColumn(1).data().toString();
    }
}

void AvailableAccounts::on_buttonBox_rejected()
{
    this->selected_account = "";
}
