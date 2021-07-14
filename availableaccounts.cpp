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
    manager->post(request, Util::generate_rpc_call("eth_accounts", "", "", "", 0, 0, 94, -1));
}

void AvailableAccounts::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error (available accounts): " << this->type << ": " << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonObject obj = Util::ObjectFromString(answer);
    QJsonArray res = obj["result"].toArray();

    qDebug() << "Available accounts: " << this->type << ": " << answer;

    this->ui->tableWidget_accounts->clear();
    while (ui->tableWidget_accounts->rowCount() > 0)
    {
        ui->tableWidget_accounts->removeRow(0);
    }

    this->ui->tableWidget_accounts->setRowCount(res.size());
    this->ui->tableWidget_accounts->setColumnCount(2);

    this->ui->tableWidget_accounts->setColumnWidth(0, 30);
    this->ui->tableWidget_accounts->setColumnWidth(1, 250);

    if(res.size() > 0){
        for(int i = 0;i<res.size();i++){
            this->ui->tableWidget_accounts->setItem( i, 0, new QTableWidgetItem(QString::number(i+1)));
            this->ui->tableWidget_accounts->setItem( i, 1, new QTableWidgetItem(res[i].toString()));
        }
    }
    this->ui->tableWidget_accounts->horizontalHeader()->setStretchLastSection(true);

    this->ui->tableWidget_accounts->setHorizontalHeaderItem(0, new QTableWidgetItem("#"));
    this->ui->tableWidget_accounts->setHorizontalHeaderItem(1, new QTableWidgetItem("Address"));
    this->ui->tableWidget_accounts->horizontalHeader()->show();

}

void AvailableAccounts::on_buttonBox_accepted()
{
    this->selected_account = "";
    QItemSelectionModel *select = this->ui->tableWidget_accounts->selectionModel();

    if(select->hasSelection()){
        this->selected_account = select->selectedRows().at(0).siblingAtColumn(1).data().toString();
    }
}

void AvailableAccounts::on_buttonBox_rejected()
{
    this->selected_account = "";
}
