#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileInfo>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QtWidgets>
#include <QtGui>
#include <QIcon>
#include <QtConcurrent/QtConcurrent>
#include <QtCore>


bool fileExists(QString path) {
    QFileInfo check_file(path);

    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "started";

    this->data = new Data();

    data->load_settings();

    this->node = new Node(this->data, "ethereum");
    this->binance_node = new Node(this->data, "binance");

    this->config = new Config();

    if(!fileExists("config.conf")){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Config Missing", "No Configuration File Found. Create one now?", QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
            acria_config->show();
          } else {

          }
    }
    else{
        if(!this->config->parseConfig()){
            qDebug() << "Error config";
        }
    }

    this->acria_config = new AcriaConfig(this, this->config);
    this->processing_window = new ProcessingWindow();

    this->tasks = new Tasks(this->data, "ethereum");
    this->binance_tasks = new Tasks(this->data, "binance");

    this->cinfo = new compinfo(this->data, "ethereum");
    this->binance_cinfo = new compinfo(this->data, "binance");

    this->igeth = new InfoGeth(this, this->node, this->data, "ethereum");
    this->binance_igeth = new InfoGeth(this, this->binance_node, this->data, "binance");

    this->balances = new Balances(this->data, "ethereum");
    this->binance_balances = new Balances(this->data, "binance");

    this->withdraw = new Withdraw(this->data, "ethereum");
    this->binance_withdraw = new Withdraw(this->data, "binance");

    this->deploy_window = new DeployWindow(this, this->data, "ethereum", this->processing_window, "");
    this->binance_deploy_window = new DeployWindow(this, this->data, "binance", this->processing_window, "");

    ui->setupUi(this);

    QtConcurrent::run(this, &MainWindow::get_status_geth);
    QtConcurrent::run(this, &MainWindow::get_status_polkadot);
    QtConcurrent::run(this, &MainWindow::get_status_acria);
    QtConcurrent::run(this, &MainWindow::get_status_config);
    QtConcurrent::run(this, &MainWindow::get_status_binance);

    tasks->update_tasks();

    timer_update_requests = new QTimer(this);
    connect(timer_update_requests, SIGNAL(timeout()), this, SLOT(update_requests()));
    timer_update_requests->start(INTERVAL_RUN);

    timer_update_events = new QTimer(this);
    connect(timer_update_events, SIGNAL(timeout()), this, SLOT(update_events()));
    timer_update_events->start(INTERVAL_RUN);

    timer_update_balances = new QTimer(this);
    connect(timer_update_balances, SIGNAL(timeout()), this, SLOT(update_balances()));
    timer_update_balances->start(INTERVAL_RUN);

    this->pi = new QProgressIndicator();
    this->ui->horizontalLayout->layout()->addWidget(pi);

    pi->startAnimation();
    pi->setColor(QColor::fromRgb(255,255,255));

    this->pi2 = new QProgressIndicator();
    this->ui->horizontalLayout_2->layout()->addWidget(pi2);

    pi2->startAnimation();
    pi2->setColor(QColor::fromRgb(255,255,255));

    this->update_settings();
}

void MainWindow::update_settings(){
    this->ui->lineEdit_geth_url->setText(this->data->geth_url);
    this->ui->lineEdit_eth_account->setText(this->data->eth_account);
    this->ui->lineEdit_eth_contract->setText(this->data->eth_contract);

    this->ui->lineEdit_polkadot_url->setText(this->data->polkadot_url);
    this->ui->lineEdit_polkadot_account->setText(this->data->polkadot_account);
    this->ui->lineEdit_polkadot_contract->setText(this->data->polkadot_contract);

    this->ui->lineEdit_binance_url->setText(this->data->binance_url);
    this->ui->lineEdit_binance_account->setText(this->data->binance_account);
    this->ui->lineEdit_binance_contract->setText(this->data->binance_contract);
}

void MainWindow::get_status_geth(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(node->get_status_geth()){
        ui->status_geth->setText("Ok");
    }
}

void MainWindow::get_status_polkadot(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(node->get_status_polkadot()){
        ui->status_polkadot->setText("Ok");
    }
}

void MainWindow::get_status_binance(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(binance_node->get_status_geth()){
        ui->status_binance->setText("Ok");
    }
}

void MainWindow::get_status_acria(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(node->get_status_acria()){
        ui->status_acria->setText("Ok");
    }
}

void MainWindow::get_status_config(){
    if(node->get_status_config()){
        ui->status_config->setText("Ok");
    }
}

void MainWindow::update_requests(){
    tasks->update_requests();
    binance_tasks->update_requests();

    std::vector<req> r = tasks->get_requests();

    std::vector<req> r2 = binance_tasks->get_requests();
    r.insert(r.end(), r2.begin(), r2.end());

    this->ui->tableWidget_req->clear();
    while (ui->tableWidget_req->rowCount() > 0)
    {
        ui->tableWidget_req->removeRow(0);
    }
    this->ui->tableWidget_req->setRowCount(r.size());
    qDebug() << "r " <<r.size();
    this->ui->tableWidget_req->setColumnCount(6);
    this->ui->tableWidget_req->setColumnWidth(4, 140);
    this->ui->tableWidget_req->setColumnWidth(0, 20);
    for (uint d=0; d<r.size(); d++){
        this->ui->tableWidget_req->setItem( d, 0, new QTableWidgetItem(QString::number(d)));
        this->ui->tableWidget_req->setItem( d, 1, new QTableWidgetItem(r[d].requestID));
        this->ui->tableWidget_req->setItem( d, 2, new QTableWidgetItem(QString::number(double(r[d].fee / pow(10,18)))));
        this->ui->tableWidget_req->setItem( d, 3, new QTableWidgetItem(QString::number(r[d].expiration)));
        //this->ui->tableWidget_req->setItem( d, 4, new QTableWidgetItem(r[d].callback));
        this->ui->tableWidget_req->setItem( d, 4, new QTableWidgetItem(r[d].chain));
        this->ui->tableWidget_req->setItem( d, 5, new QTableWidgetItem(QString::number(r[d].id)));
    }

    for (uint d=0; d<r.size(); d++){
            if (!std::count(nt.begin(), nt.end(), r[d].chain + QString::number(r[d].id))){
                nlohmann::json conf1 = config->get_config();
                for(uint i = 0; i<conf1.size();i++){
                    if(QString::fromStdString(conf1[i]["rname"]) == r[d].requestID){
                        std::vector<QString> l_json;

                        for(uint f = 0; f<conf1[i]["json"].size();f++){
                            l_json.push_back(QString::fromStdString(conf1[i]["json"][f]));
                        }

                        Resource* rr = new Resource(QString::fromStdString(conf1[i]["url"]), l_json, this->data->eth_contract, QString::fromStdString(conf1[i]["rname"]), this->data, r[d].chain, r[d].id);
                        rr->update_resource();

                        this->tm_resources.push_back(rr);
                        nt.push_back(r[d].chain + QString::number(r[d].id));
                    }

            }
        }
    }

    if(r.size() != 0){
        this->ui->horizontalLayoutWidget->hide();
        this->ui->label_progress->hide();
    }
    else{
        this->ui->horizontalLayoutWidget->show();
        this->ui->label_progress->show();
    }

    for(uint i=0; i<this->tm_resources.size();i++){
        if(this->tm_resources[i]->get_state() == 2){
            delete this->tm_resources[i];
            this->tm_resources.erase(this->tm_resources.begin() + i);
        }
    }
}

void MainWindow::update_events(){
    cinfo->update_events();
    binance_cinfo->update_events();

    std::vector<comp> r = cinfo->get_completed();
    std::vector<comp> r2 = binance_cinfo->get_completed();
    r.insert(r.end(), r2.begin(), r2.end());

    this->ui->tableWidget_comp->clear();
    while (ui->tableWidget_comp->rowCount() > 0)
    {
        ui->tableWidget_comp->removeRow(0);
    }

    this->ui->tableWidget_comp->setRowCount(r.size());
    this->ui->tableWidget_comp->setColumnCount(6);
    this->ui->tableWidget_comp->setColumnWidth(4, 200);
    this->ui->tableWidget_comp->setColumnWidth(0, 20);
    this->ui->tableWidget_comp->setColumnWidth(5, 30);

    if(r.size() != 0){
        this->ui->horizontalLayoutWidget_2->hide();
        this->ui->label_progress_2->hide();

        for (int d=r.size()-1; d>=0; d--){
            uint dd = r.size()-1-d;
            qDebug() << r.size() << dd << d;
            this->ui->tableWidget_comp->setItem( dd, 0, new QTableWidgetItem(QString::number(d)));
            this->ui->tableWidget_comp->setItem( dd, 1, new QTableWidgetItem(r[d].requestID));
            //this->ui->tableWidget_comp->setItem( d, 2, new QTableWidgetItem(r[d].callback));
            this->ui->tableWidget_comp->setItem( dd, 2, new QTableWidgetItem(r[d].chain));
            this->ui->tableWidget_comp->setItem( dd, 3, new QTableWidgetItem(QString::number(r[d].block)));
            this->ui->tableWidget_comp->setItem( dd, 4, new QTableWidgetItem(r[d].hash));
            this->ui->tableWidget_comp->setItem( dd, 5, new QTableWidgetItem(QString::number(double(r[d].fee / pow(10,18)))));
        }
    }
    else{
        this->ui->horizontalLayoutWidget_2->show();
        this->ui->label_progress_2->show();
    }
}

void MainWindow::update_balances(){
    this->balances->update_withdrawable();
    this->binance_balances->update_withdrawable();

    this->ui->tableWidget_balances->clear();
    while (ui->tableWidget_balances->rowCount() > 0)
    {
        ui->tableWidget_balances->removeRow(0);
    }

    this->ui->tableWidget_balances->setRowCount(5);
    this->ui->tableWidget_balances->setColumnCount(4);

    this->ui->tableWidget_balances->setColumnWidth(1, 100);
    this->ui->tableWidget_balances->setColumnWidth(2, 100);
    this->ui->tableWidget_balances->setColumnWidth(3, 120);

    this->ui->tableWidget_balances->setItem( 0, 0, new QTableWidgetItem(""));
    this->ui->tableWidget_balances->setItem( 1, 0, new QTableWidgetItem("Ethereum"));
    this->ui->tableWidget_balances->setItem( 2, 0, new QTableWidgetItem("Polkadot"));
    this->ui->tableWidget_balances->setItem( 3, 0, new QTableWidgetItem("Binance"));
    this->ui->tableWidget_balances->setItem( 4, 0, new QTableWidgetItem("Total"));
    this->ui->tableWidget_balances->setItem( 0, 1, new QTableWidgetItem("Fees Earned Total"));
    this->ui->tableWidget_balances->setItem( 0, 2, new QTableWidgetItem("Withdrawable Fees"));
    this->ui->tableWidget_balances->setItem( 0, 3, new QTableWidgetItem("Completed Requests"));

    uint eth_completed = this->cinfo->get_completed().size();
    uint binance_completed = this->binance_cinfo->get_completed().size();
    uint polkadot_completed = 0;

    double binance_total_fees = this->binance_cinfo->get_total_fees();
    double eth_total_fees = this->cinfo->get_total_fees();
    double polkadot_total_fees = 0;

    double binance_withdrawable = this->binance_balances->get_withdrawable();
    double eth_withdrawable = this->balances->get_withdrawable();
    double polkadot_withdrawable = 0;

    this->ui->tableWidget_balances->setItem( 1, 3, new QTableWidgetItem(QString::number(eth_completed)));
    this->ui->tableWidget_balances->setItem( 2, 3, new QTableWidgetItem(QString::number(polkadot_completed)));
    this->ui->tableWidget_balances->setItem( 3, 3, new QTableWidgetItem(QString::number(binance_completed)));
    this->ui->tableWidget_balances->setItem( 4, 3, new QTableWidgetItem(QString::number(binance_completed + polkadot_completed + eth_completed)));

    this->ui->tableWidget_balances->setItem( 3, 1, new QTableWidgetItem(QString::number(binance_total_fees)));
    this->ui->tableWidget_balances->setItem( 2, 1, new QTableWidgetItem(QString::number(polkadot_total_fees)));
    this->ui->tableWidget_balances->setItem( 1, 1, new QTableWidgetItem(QString::number(eth_total_fees)));

    this->ui->tableWidget_balances->setItem( 3, 2, new QTableWidgetItem(QString::number(binance_withdrawable)));
    this->ui->tableWidget_balances->setItem( 2, 2, new QTableWidgetItem(QString::number(polkadot_withdrawable)));
    this->ui->tableWidget_balances->setItem( 1, 2, new QTableWidgetItem(QString::number(eth_withdrawable)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete acria_config;
    delete processing_window;

    delete node;
    delete tasks;
    delete cinfo;
    delete pi;
    delete pi2;
    delete data;
    delete igeth;

    delete binance_cinfo;
    delete binance_tasks;

    delete timer_update_requests;
    delete timer_update_events;
    delete timer_update_balances;

    delete balances;
    delete binance_balances;

    delete withdraw;
    delete binance_withdraw;

    delete binance_node;

    delete binance_deploy_window;
    delete deploy_window;

    for(uint i=0; i<this->tm_resources.size();i++){
        delete this->tm_resources[i];
    }
}


void MainWindow::on_pushButton_export_json_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);


    if(dir != ""){
        std::vector<comp> r = cinfo->get_completed();
        std::vector<comp> r2 = binance_cinfo->get_completed();
        r.insert(r.end(), r2.begin(), r2.end());

        nlohmann::json tmp;

        for(uint i = 0; i<r.size(); i++){
            tmp[i]["requestID"] = r[i].requestID.toStdString();
            tmp[i]["callback"] = r[i].callback.toStdString();
            tmp[i]["hash"] = r[i].hash.toStdString();
            tmp[i]["block"] = r[i].block;
            tmp[i]["address"] = r[i].address.toStdString();
        }

        QString filename=dir + "/transactions.json";
        QFile file(filename);

        qDebug() << filename;

        if(!file.exists()){
            qDebug() << "File exists: "<<filename;
        }else{
            qDebug() << filename<<" does not exist";
        }

       if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);
            out << QString::fromStdString(tmp.dump());
            file.close();
       }

       QMessageBox msgBox;
       msgBox.setText("Successfully exported as Json!");
       msgBox.exec();
    }
}

void MainWindow::on_pushButton_export_csv_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if(dir != ""){
        std::vector<comp> r = cinfo->get_completed();
        std::vector<comp> r2 = binance_cinfo->get_completed();
        r.insert(r.end(), r2.begin(), r2.end());

        QString tmp = "";
        QString delim = ";";
        tmp += "requestID"+delim+"callback"+delim+"hash"+delim+"block"+delim+"address\n";

        for(uint i = 0; i<r.size(); i++){
            tmp += QString::fromStdString(r[i].requestID.toStdString());
            tmp += delim;
            tmp += QString::fromStdString(r[i].callback.toStdString());
            tmp += delim;
            tmp += QString::fromStdString(r[i].hash.toStdString());
            tmp += delim;
            tmp += QString::number(r[i].block);
            tmp += delim;
            tmp += QString::fromStdString(r[i].address.toStdString());
            tmp += "\n";
        }

        QString filename=dir + "/transactions.csv";
        QFile file(filename);

        if(!file.exists()){
            qDebug() << "File exists: "<<filename;
        }else{
            qDebug() << filename<<" does not exist";
        }

       if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);
            out << tmp;
            file.close();
       }

       QMessageBox msgBox;
       msgBox.setText("Successfully exported as CSV!");
       msgBox.exec();
    }
}

void MainWindow::on_pushButton_export_json_3_clicked()
{
    if(this->acria_config->exec() == QDialog::Accepted){

    }
}

void MainWindow::on_pushButton_refresh_clicked()
{
    ui->status_geth->setText("...");
    ui->status_polkadot->setText("...");
    ui->status_acria->setText("...");
    ui->status_config->setText("...");

    this->node->update_geth_status();
    this->binance_node->update_geth_status();

    QtConcurrent::run(this, &MainWindow::get_status_geth);
    QtConcurrent::run(this, &MainWindow::get_status_polkadot);
    QtConcurrent::run(this, &MainWindow::get_status_acria);
    QtConcurrent::run(this, &MainWindow::get_status_config);
}

void MainWindow::on_pushButton_eth_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(4);
    this->ui->tabWidget_2->setCurrentIndex(0);
}

void MainWindow::on_pushButton_polkadot_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(4);
    this->ui->tabWidget_2->setCurrentIndex(1);
}

void MainWindow::on_pushButton_acria_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(4);
}

void MainWindow::on_pushButton_config_settings_clicked()
{
    this->acria_config->show();
}

void MainWindow::on_pushButton_setting_save_clicked()
{
    this->data->binance_url = this->ui->lineEdit_binance_url->text();
    this->data->binance_account = this->ui->lineEdit_binance_account->text();
    this->data->binance_contract = this->ui->lineEdit_binance_contract->text();

    this->data->geth_url = this->ui->lineEdit_geth_url->text();
    this->data->eth_account = this->ui->lineEdit_eth_account->text();
    this->data->eth_contract = this->ui->lineEdit_eth_contract->text();

    this->data->polkadot_url = this->ui->lineEdit_polkadot_url->text();
    this->data->polkadot_account = this->ui->lineEdit_polkadot_account->text();
    this->data->polkadot_contract = this->ui->lineEdit_polkadot_contract->text();

    this->data->save_settings();

    this->cinfo->create_filter_events();
    this->binance_cinfo->create_filter_events();

    QMessageBox msgBox;
    msgBox.setText("Successfully saved the settings!");
    msgBox.exec();
}

void MainWindow::on_pushButton_setting_discard_clicked()
{
    this->data->load_settings();
    this->update_settings();
}

void MainWindow::on_pushButton_eth_info_clicked()
{
    this->igeth->update_info();

    this->igeth->exec();
}

void MainWindow::on_pushButton_binance_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(4);
    this->ui->tabWidget_2->setCurrentIndex(2);
}

void MainWindow::on_pushButton_withdraw_eth_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Withdraw", "Withdraw everything from the Binance contract?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          this->processing_window->show();
        this->withdraw->withdraw(processing_window);
      } else {

      }
}

void MainWindow::on_pushButton_withdraw_polkadot_clicked()
{

}

void MainWindow::on_pushButton_withdraw_binance_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Withdraw", "Withdraw everything from the Binance contract?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          this->processing_window->show();
        this->binance_withdraw->withdraw(processing_window);
      } else {

      }
}

void MainWindow::on_pushButton_binance_info_clicked()
{
    this->binance_igeth->update_info();

    this->binance_igeth->exec();
}

void MainWindow::on_pushButton_deploy_contract_binance_clicked()
{
     binance_deploy_window->exec();
}

void MainWindow::on_pushButton_deploy_contract_eth_clicked()
{
    deploy_window->exec();
}
